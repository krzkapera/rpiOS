# Systemy Wbudowane 2025 RPi OS


Celem projektu było poznanie zasad budowy systemów operacyjnych, programowania niskopoziomowego bare-metal i platformy sprzętowej Raspberry Pi 4B podczas implementacji własnego kernela systemu operacyjnego. Udało się zrealizować wszystkie zakładane funkcje, oprócz pamięci wirtualnej. System pozwala na uruchomienie kliku procesów użytkownika. Nadzoruje ich dostęp do UART, przydziela pamięć oraz realizuje przełączanie procesów. Dodatkowo, dla wygody programistycznej, zaimplementowano prosty protokół komunikacji przez UART, który pozwala na przesłanie na platformę sprzętową nowej wersji systemu. Do tego potrzebna była obsługa kontrolera karty SD i systemu plików.

## Zrealizowane funkcje

- Komunikacja UART
- System plików (kernela)
- Obsługa przerwań
- Przełączanie zadań
- Wywołania systemowe
- Wgrywanie aktualizacji

## Dokumentacja

Do uruchomienia projektu potrzebna jest płytka Raspberry Pi 4B, przejściówka UART-USB, karta microSD oraz adapter microSD-USB. Na kartę SD należy wgrać Raspberry Pi OS. Wynikiem kompilacji jest plik *kernel8.img*, którym należy zastąpić plik na karcie SD o tej samej nazwie.

Do tego celu zdefiniowano w *Makefile* dwa polecenia: `make sd` oraz `make pi`. Oba działają pod środowiskiem Microsoft Windows WSL. Pierwsze zapisuje plik na karcie SD podłączonej przez adapter, drugie za pomocą skryptu w języku Python *update.py* przesyła nową wersję systemu przez UART. Dołączony został również skrypt *uart.sh*, który odczytuje dane przesyłane tym kanałem.

W pliku *config.txt* należy ustawić:
```
kernel_old=1
kernel_address=0x0
```
`kernel_old` powoduje, że system się uruchamia na *Exception level* 3 zamiast 2.

### Struktura projektu

Pliki projektu są zorganizowane w następujący sposób:

- **fork/** - tworzenie nowego procesu
- **io/** - podstawowe funkcje do zapisu/odczytu peryferiów
   - **uart/** - odczyt/zapis kanału uart; funkcje `printf()`, `puts()`
- **irq/** - obsługa przerwań, definicja tabeli wektorów przerwań
- **mem/** - przydział i zwalnianie pamięci
- **power/** - funkcja `restart()` korzystająca z zegara *watchdog*
- **scheduler/** - przełączanie procesów, zmiana kontekstu
- **sd/** - obsługa kontrolera *EMMC* karty SD
   - **fatfs/** - system plików karty SD; biblioteka *FatFS*
- **syscalls/** - przerwania systemowe
- **system_loader/** - pobranie, walidacja, zapis aktualizacji systemu
- **system_timer/** - inicjalizacja, odczyt zegara systemowego
- **user/** - funkcje przykładowych procesów użytkownika
- *kernel.c* - inicjalizacja systemu, uruchomienie procesów użytkownika
- *boot.S* - przygotowanie rejestrów dla kernela

Ważne i ciekawe moduły zostały omówione poniżej.

### Plik `boot.S`

W tym pliku konfigurowane jest środowisko pod działanie kernela.

Na początku sprawdzane jest, na którym rdzeniu uruchomił się kod. Rejestr `mpidr_el1` *Multiprocessor Affinity Register* zawiera informacje o tym, który rdzeń wykonuje kod. Jeżeli nie jest to główny rdzeń, to wykonanie zawiesza się w nieskończonej pętli. Zapobiega to różnym problemom w sytuacji, w której ten sam kod wykonywałby się wiele razy na różnych rdzeniach. Poza tym, wtedy z innych rdzeni można korzystać wg własnych potrzeb. 

Następnie włączany jest dostęp do rejestrów zmiennoprzecinkowych. Potrzebuje tego funkcja `printf()`, która przyjmuje w nich argumenty (choć można też kompilować z opcją `-mgeneral-regs-only`).

Ostatnią rzeczą jest przejście z *Exception level* 3 do 1. Robi się to za pomocą instrukcji `eret`, która oznacza powrót z wyjątku, ale można też jej użyć do zmiany EL, jeżeli dobrze skonfiguruje się rejestry. W rejestrze `sp_el1` ustawiany jest wskaźnik stosu, w `hcr_el2` ustawia się tryb 64-bitowy, w `scr_el3` tryb non-secure, `spsr_el3` określa przejście do EL 1. W `elr_el3` ustawiany jest adres miejsca, które wywołuje funkcję `main()` z *kernel.c* i wykonywany jest skok.


### Obsługa przerwań

Obsługa przerwań to jedna z ważniejszych części systemu. Pozwala zmieniać wykonywany proces co zadany odstęp czasu oraz wykryć moment, w którym przez UART przyjdą dane oraz zaimplementować wywołania systemowe.

W rejestrze `vbar_el1` należy zapisać adres *Interrupt Vector Table*, czyli miejsca w pamięci, w którym dla każdego przerwania określona jest funkcja, która ma być wywołana. Robi to funkcja `irq_init_vectors` z pliku *irq.S*, a IVT jest określona w *entry.S*. Wykorzystane jest tylko kilka przerwań, dla reszty podana jest funkcja, która wypisuje błąd. Nieużywane przerwania i tak nigdy nie powinny się zdarzyć.

Dla przerwań asynchronicznych IRQ użyta jest generyczna funkcja `handle_irq()`, która obsługuje przerwania UART oraz zegara systemowego. Na podstawie bitów rejestru IRQ0_PENDING0 rozpoznaje typ przerwania i podejmuje odpowiednią akcję.

Przerwania synchroniczne różnią się tym, że są wywoływane z kodu przez instrukcję `svc`, a nie przez zewnętrzne akcje. Mogą przyjmować argumenty. W projekcie posłużyły do zaimplementowania wywołań systemowych. Funkcje do ich obsługi, tak samo, określa się w IVT.

Warto wspomnieć, że obsługa każdego przerwania musi zaczynać się od zapisania stanu procesora (makro `kernel_entry`) i kończyć jego przywróceniem (`kernel_exit`), aby nie utracić danych w trakcie obsługi przerwania.

### Przełączanie zadań

Każde zadanie opisane jest strukturą:

```C
struct task_struct {
	struct cpu_context cpu_context; // stan procesora, wartości rejestrów
	int64_t state;                  // stan zadania, RUNNING albo ZOMBIE
	int64_t counter;                // zlicza, jak długo zadanie się wykonuje
	int64_t priority;               // priorytet, jak często ma dostawać procesor
	int64_t preempt_count;          // określa, czy można przerwać zadanie
	uint64_t stack;                 // adres stosu procesu
	uint64_t flags;                 // dodatkowe opcje
};
```

Przełączanie zadań (procesów) jest realizowane za pomocą adaptacji funkcji `schedule()` z [4]. Jest ona wywoływana za każdym przerwaniem zegara systemowego i wybiera proces, który wykonywał się najkrócej lub ten o największym priorytecie. Przełączenie kontekstu, wykonywane przez `cpu_switch_to()` z *scheduler.S*, polega na zapisaniu stanu poprzedniego procesu i wczytanie do rejestrów stanu nowego procesu.

Tworzeniem nowego procesu zajmuje się funkcja `copy_process()` z *fork.c*. Po kolei musi ona: wstrzymać przełączanie na czas tworzenia procesu, zaalokować dla niego pamięć, utworzyć jego reprezentację jako `task_struct` i dodać do listy zadań.


### Wywołania systemowe

Wywołania systemowe są zrealizowane za pomocą przerwań synchronicznych. Każde wywołanie polega na przygotowaniu argumentów, zapisaniu numeru wywołania i wykonanie instrukcji `svc`, która generuje przerwanie synchroniczne.

Obsługa takiego przerwania polega na walidacji argumentów i wywołaniu odpowiedniej funkcji z tabeli `sys_call_table` zdefiniowanej w *syscalls.c*.

Określone zostały wywołania do zapisu bufora na UART, zaalokowania nowej pamięci i zakończenia procesu.

### Wgrywanie aktualizacji

Obsługa aktualizacji po stronie RPi jest w całości zdefiniowana w *system_loader.c*. Gdy zostanie wykryte przerwanie przychodzące UART wywoływana jest funkcja `read_data()`, która odczytuje całość danych zapisując je w buforze. Sprawdza przy tym poprawność sumy kontrolnej. Jeżeli się zgadza, to plik *kernel8.img* jest zapisywany na karcie SD z otrzymaną treścią, a RPi jest restartowane.

Przesyłanie obrazu systemu realizuje skrypt *update.py*. Po dopisaniu nowej funkcjonalności (lub naprawieniu buga, to częściej) wystarczy wpisać polecenie `make pi`, które jest aliasem na `make && python3 update.py`. Skrypt oblicza sumę kontrolną i razem z danymi wysyła przez UART.

Dane są przesyłane w postaci:
```
| długość danych | checksuma długości | checksuma danych | 0xAA | dane | 0xAA |
```

Długość zajmuje 4 bajty, a sumy kontrolne po 1. Dodanie bajtów `0xAA`, wraz z informacją o liczbie odczytanych bajtów, pozwala wykryć kluczowe momenty transmisji, czyli początek i koniec danych. Gdy zostanie wykryt bajt `0xAA` i liczba odczytanych bajtów to 7, to wiadomo, że otrzymano nagłówek; można sprawdzić sumę kontrolną długości i czekać na dane. Analogicznie wykrywany jest koniec danych i całej transmisji.

#### Obsługa karty SD

Zapisywanie pliku na karcie SD to, wbrew pozorom, skomplikowana rzecz. W tym celu należy zaimplementować sterownik EMMC karty SD oraz obsługę systemu plików karty.

Kod sterownika EMMC w całości pochodzi z [3]. Kluczowe są dwie modyfikacje:

- RPi 4, w stosunku do RPi 3, wprowadza nowy kontroler EMMC2. Na szczęście poprzedni został zachowany, należy się na niego przełączyć funkcją `use_legacy_sd_controller()`.
```C
void use_legacy_sd_controller() {
	long addr = PERIPHERAL_BASE + 0x2000d0;
	mmio_write(addr, mmio_read(addr) | 0b10);
}
```
- Nie należy zwiększać częstotliwości zegara karty, jak to zrobiono w [3].

Mając gotowe funkcje do odczytu/zapisu bloków karty SD oraz wysyłania komend kontrolera bardzo łatwo zaimplementować obsługę systemu plików przy użyciu biblioteki *FatFs*. W `sd/fatfs/diskio.c` wystarczy użyć właśnie tych funkcji do zapisu/odczytu odpowiedniego sektora. Resztą zajmuje się biblioteka.



## Możliwe dalsze ulepszenia

- Pamięć wirtualna :(
- Wczytywanie programów użytkownika z plików
- Wykorzystanie wszystkich rdzeni; wielowątkowość
- Dostęp do plików dla procesów użytkownika
- Sterowniki: dostępu do sieci, ekranu, urządzeń USB
- Rust

### Pamięć wirtualna
Próbowałem wg [1] i [2]. Nie da się debugować, gdy nie działa dostęp do UART. Trzeba taki system najpierw uruchamiać na emulatorze.



## Źródła

- https://github.com/s-matyukevich/raspberry-pi-os [1]
- https://github.com/rhythm16/rpi4-bare-metal [2]
- https://github.com/bztsrc/raspi3-tutorial [3]
- https://github.com/babbleberry/rpi4-osdev
- https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf
- https://developer.arm.com/documentation/100095/latest/
- https://github.com/zavg/linux-0.01/blob/master/kernel/sched.c#L68 [4]
