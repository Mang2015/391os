/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
    uint32_t flags;
    cli_and_save(flags);

    master_mask = 0xFF;
    slave_mask = 0xFF;


    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_PORT_DATA);
    outb(ICW3_MASTER, MASTER_8259_PORT_DATA);

    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_PORT_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_PORT_DATA);

//    outb(ICW4, MASTER_8259_PORT_DATA);
    outb(ICW4, SLAVE_8259_PORT_DATA);

    outb(master_mask, MASTER_8259_PORT_DATA); // mask master PIC
    outb(slave_mask, SLAVE_8259_PORT_DATA); // mask slave PIC

    enable_irq(SLAVE_IRQ_NUM);

    restore_flags(flags);
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
    if(irq_num < 8)
    {
        master_mask = inb(MASTER_8259_PORT_DATA) & ~(1 << irq_num);
        outb(master_mask, MASTER_8259_PORT_DATA);
    }
    else
    {
        irq_num -= 8;
        slave_mask = inb(SLAVE_8259_PORT_DATA) & ~(1 << irq_num);
        outb(slave_mask, SLAVE_8259_PORT_DATA);
    }
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
    if(irq_num < 8)
    {
        master_mask = inb(MASTER_8259_PORT_DATA) | (1 << irq_num);
        outb(master_mask, MASTER_8259_PORT_DATA);
    }
    else
    {
        irq_num -= 8;
        slave_mask = inb(SLAVE_8259_PORT_DATA) | (1 << irq_num);
        outb(slave_mask, SLAVE_8259_PORT_DATA);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
    uint8_t PIC_EOI;

    if(irq_num < 8)
    {
        PIC_EOI = EOI | irq_num;
        outb(PIC_EOI, MASTER_8259_PORT);
    }
    else
    {
        irq_num -= 8;
        PIC_EOI = EOI | irq_num;
        outb(EOI | SLAVE_IRQ_NUM, MASTER_8259_PORT);
        outb(PIC_EOI, SLAVE_8259_PORT);
    }
}
