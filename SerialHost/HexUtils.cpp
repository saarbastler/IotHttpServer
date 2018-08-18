/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HexUtils.cpp
 * Author: Joerg
 * 
 * Created on 6. März 2016, 16:09
 */

#include <stdio.h>

#include "HexUtils.h"

void HexUtils::printHex(const char * prefix, const unsigned char *buffer, int count)
{
  if (prefix != NULL)
    printf("%s", prefix);

  for (int i = 0; i < count; i++)
  {
    printf("%02X ", (int) (buffer[i]&0xff));
  }
  printf("\n");

}