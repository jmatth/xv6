.globl read_PIT_count
read_PIT_count:
  pushf
  cli
  mov $0x00000000b, %al  # al = channel in bits 6 and 7, remaining bits clear
  out %al, $0x43         # Send the latch command

  in $0x40, %al          # al = low byte of count
  mov %al, %ah           # ah = low byte of count
  in $0x40, %al          # al = high byte of count
  rol $8, %ax            # al = low byte, ah = high byte (ax = current count)
  popf
  ret
