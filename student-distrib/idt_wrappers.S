
.globl keyboard_handler_wrapper
.globl rtc_handler_wrapper

# keyboard_handler_wrapper
# Description: wrapper for keyboard interrupt handler to follow proper
#               interrupt stack convention
# INPUT/OUTPUT: none
# SIDE EFFECTS: none
keyboard_handler_wrapper:
  pusha
  call keyboard_handler
  popa
  iret

# rtc_handler_wrapper
# Description: wrapper for rtc interrupt handler to follow proper
#               interrupt stack convention
# INPUT/OUTPUT: none
# SIDE EFFECTS: none
rtc_handler_wrapper:
  pusha
  call rtc_handler
  popa
  iret
