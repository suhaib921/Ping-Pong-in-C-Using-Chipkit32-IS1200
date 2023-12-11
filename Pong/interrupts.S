#Header-fil till data.c som kompilerades av F Lundevall ar 2015. Originalkod fran Axel Isaksson. Se COPYING.txt for upphovsratt och licensiering.

.text
.global enable_interrupt
enable_interrupt:
  ei                        # enable interrupts globally 
  jr    $ra                 # return
  nop                       # delay slot filler
