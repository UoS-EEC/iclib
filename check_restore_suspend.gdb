set pagination off
set verbose off
tui disable

b *0x80008c6
b *0x80008ca

target remote :51000
load
c

set $i = 0
while ($i < 1)
set $i = $i + 1
p "------ New iteration ------"
p "------ suspend "
#Breakpoint at suspend
info registers
set $len = (uint32_t *)&__stack_high - (uint32_t *)$sp
p/x *(uint32_t *)$sp@$len
c
# Breakpoint at restore
p "------ restore "
info registers
set $len = (uint32_t *)&__stack_high - (uint32_t *)$sp
p/x *(uint32_t *)$sp@$len
# Check equality
p $ == $$2
end
