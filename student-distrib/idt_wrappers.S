


.globl keyboard_handler_wrapper
.globl rtc_handler_wrapper

keyboard_handler_wrapper:
  pusha
  call keyboard_handler
  popa
  iret

rtc_handler_wrapper:
  pusha
  call rtc_handler
  popa
  iret
