[ ! -p /tmp/mypipe ] && mkfifo /tmp/mypipe
tail -f /tmp/mypipe | ../plink.exe -serial COM7 -sercfg 115200,8,n,1,N
