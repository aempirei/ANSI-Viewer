VAR

  long stack[16]
  byte cog
  
PUB start(pin,count,delayms, speed_ptr): success

  stop
  success := (cog := cognew(handler(pin,count,delayms,speed_ptr), @stack) + 1)

PUB stop

  if cog
    cogstop(cog~ - 1)

PUB handler(pin,count,delayms,speed_ptr)

  blink(pin,count,delayms)

  dira[8]~
  dira[9]~

  repeat

    if ina[8]

      word[speed_ptr] := (word[speed_ptr] * 3 / 4) #> 64

      blink(pin,1,100)

      repeat while ina[8]

    elseif ina[9]
    
      word[speed_ptr] := (word[speed_ptr] * 5 / 4) <# 2048

      blink(pin,1,100)

      repeat while ina[9]

  cog~

PUB blink(pin,count,delayms) | ticks, i

  dira[pin]~~

  ticks := delayms * (clkfreq / 1000) ' delayms * clkfreq / 1000

  repeat i from 1 to count * 2
      outa[pin] := i // 2
      waitcnt(ticks + cnt)