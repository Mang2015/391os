
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

# system_handler_wrapper
# Description: wrapper for system interrupt handler to follow proper
#               interrupt stack convention
# INPUT/OUTPUT: none
# SIDE EFFECTS: none
system_handler_wrapper:
  pushl %ecx
  pushl %edx
  pushl %ebx
  pushl %esp
  pushl %ebp
  pushl %esi
  pushl %edi
  call system_handler
  popl %edi
  popl %esi
  popl %ebp
  popl %esp
  popl %ebx
  popl %edx
  popl %ecx
  # preserve eax as return value
  iret
