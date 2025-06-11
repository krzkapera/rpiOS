# Systemy Wbudowane 2025 RPi OS


Celem projektu było poznanie zasad budowy systemów operacyjnych, programowania niskopoziomowego bare-metal i platformy sprzętowej Raspberry Pi 4B podczas implementacji własnego kernela systemu operacyjnego. Udało się zrealizować wszystkie zakładane funkcje, oprócz pamięci wirtualnej. System pozwala na uruchomienie kliku procesów użytkownika. Nadzoruje ich dostęp do UART, przydziela pamięć oraz realizuje przełączanie procesów. Dodatkowo, dla wygody programistycznej, zaimplementowano prosty protokół komunikacji przez UART, który pozwala na przesłanie na platformę sprzętową nowej wersji systemu. Do tego potrzebna była obsługa kontrolera karty SD i systemu plików.

## Zrealizowane funkcje

- Komunikacja UART
- System plików
- Obsługa przerwań
- Przełączanie zadań
- Wywołania systemowe
- Wgrywanie aktualizacji

## Dokumentacja

Do uruchomienia projektu potrzebna jest płytka Raspberry Pi 4B, przejściówka UART-USB, karta microSD oraz adapter microSD-USB. Na kartę SD należy wgrać Raspberry Pi OS. Wynikiem kompilacji jest plik *kernel8.img*, którym należy zastąpić plik na karcie SD o tej samej nazwie.

Do tego celu zdefiniowano w *Makefile* dwa polecenia: `make sd` oraz `make pi`. Oba działają pod środowiskiem Microsoft Windows WSL. Pierwsze zapisuje plik na karcie SD podłączonej przez adapter, drugie za pomocą skryptu w języku Python *update.py* przesyła nową wersję systemu przez UART. Dołączony został również skrypt *uart.sh*, który odczytuje dane przesyłane tym kanałem.

TODO: config.txt

### Struktura projektu

Pliki projektu są zorganizowane w następujący sposób:

- **fork/** - tworzenie nowego procesu
- **io/** - podstawowe funkcje do zapisu/odczytu peryferiów
   - **uart/** - odczyt/zapis kanału uart; funkcje `printf`, `puts`
- **irq/** - obsługa przerwań, definicja tabeli wektorów przerwań
- **mem/** - przydział i zwalnianie pamięci
- **power/** - funkcja `restart` korzystająca z zegara *watchdog*
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

Następnie włączany jest dostęp do rejestrów zmiennoprzecinkowych. Potrzebuje tego funkcja `printf`, która przyjmuje w nich argumenty (choć można też kompilować z opcją `-mgeneral-regs-only`).

Ostatnią rzeczą jest przejście z *Exception level* 3 do 1. Robi się to za pomocą instrukcji `eret`, która oznacza powrót z wyjątku, ale można też jej użyć do zmiany EL, jeżeli dobrze skonfiguruje się rejestry. W rejestrze `sp_el1` ustawiany jest wskaźnik stosu, w `hcr_el2` ustawia się tryb 64-bitowy, w `scr_el3` tryb non-secure, `spsr_el3` określa przejście do EL 1. W `elr_el3` ustawiany jest adres miejsca, które wywołuje funkcję `main` z *kernel.c* i wykonywany jest skok.


### Obsługa przerwań

Obsługa przerwań to jedna z ważniejszych części systemu. Pozwala zmieniać wykonywany proces co zadany odstęp czasu oraz wykryć moment, w którym przez UART przyjdą dane.

W rejestrze `vbar_el1` należy zapisać adres *Interrupt Vector Table*, czyli miejsca w pamięci, w którym dla każdego przerwania określona jest funkcja, która ma być wywołana. Robi to funkcja `irq_init_vectors` z pliku *irq.S*, a IVT jest określona w *entry.S*. Wykorzystane jest tylko kilka przerwań, dla reszty podana jest funkcja, która wypisuje błąd. Nieużywane przerwania i tak nigdy nie powinny się zdarzyć.

Dla przerwań synchronicznych IRQ użyta jest generyczna funkcja `handle_irq`, która obsługuje przerwania UART oraz zegara systemowego. Na podstawie bitów rejestru IRQ0_PENDING0 rozpoznaje typ przerwania i podejmuje odpowiednią akcję.

Przerwania synchroniczne ... TODO 

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

Przełączanie zadań (procesów) jest realizowane za pomocą adaptacji funkcji `schedule` z [3]. Jest ona wywoływana za każdym przerwaniem zegara systemowego i wybiera proces, który wykonywał się najkrócej lub ten o największym priorytecie. Przełączenie kontekstu, wykonywane przez `cpu_switch_to` z *scheduler.S*, polega na zapisaniu stanu poprzedniego procesu i wczytanie do rejestrów stanu nowego procesu.

Tworzeniem nowego procesu zajmuje się funkcja `copy_process` z *fork.c*. ... TODO


### Wywołania systemowe

Wywołania systemowe są zrealizowane za pomocą przerwań synchronicznych. Każde wywołanie polega na przygotowaniu argumentów, zapisaniu numeru wywołania i wykonanie instrukcji `svc`, która generuje przerwanie synchroniczne.

Obsługa takiego przerwania polega na walidacji argumentów i wywołaniu odpowiedniej funkcji z tabeli `sys_call_table` zdefiniowanej w *syscalls.c*.


### Wgrywanie aktualizacji

#### Obsługa karty SD




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
- https://github.com/bztsrc/raspi3-tutorial
- https://github.com/babbleberry/rpi4-osdev
- https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf
- https://developer.arm.com/documentation/100095/latest/
- https://github.com/zavg/linux-0.01/blob/master/kernel/sched.c#L68 [3]