// tested on STM32G431RB
// copy to main.c, section USER CODE 0

int __io_putchar(int ch)
{
  uint8_t c[1];
  c[0] = ch & 0x00FF;
  HAL_UART_Transmit(&huart2, &*c, 1, 10);
  return ch;
}

int _write(int file,char *ptr, int len)
{
  int index;
  for(index= 0; index< len; index++)
  {
    __io_putchar(*ptr++);
  }
  return len;
}
