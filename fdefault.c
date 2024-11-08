/****************************************************************************
*
*   Code to provide default (nul) behavior for backends.
*   Copyright (C) 2023 Sean Conner
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Comments, questions and criticisms can be sent to: sean@conman.org
*
****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "a09.h"

/**************************************************************************/

bool fdefault_cmdline(struct format *fmt,struct a09 *a09,struct arg *arg,char c)
{
  (void)fmt;
  (void)a09;
  (void)arg;
  (void)c;
  return false;
}

/**************************************************************************/

bool fdefault_pass(struct format *fmt,struct a09 *a09,int pass)
{
  (void)fmt;
  (void)pass;
  (void)a09;
  return true;
}

/**************************************************************************/

bool fdefault(struct format *fmt,struct opcdata *opd)
{
  (void)fmt;
  (void)opd;
  return true;
}

/**************************************************************************/

bool fdefault_end(struct format *fmt,struct opcdata *opd,struct symbol const *sym)
{
  (void)fmt;
  (void)opd;
  (void)sym;
  return true;
}

/**************************************************************************/

bool fdefault_write(
        struct format  *fmt,
        struct opcdata *opd,
        void const     *buffer,
        size_t          len,
        bool            instruction
)
{
  (void)fmt;
  assert(opd       != NULL);
  assert(opd->pass == 2);
  assert(buffer    != NULL);
  (void)instruction;
  
  if (fwrite(buffer,1,len,opd->a09->out) != len)
  {
    if (ferror(opd->a09->out))
      return message(opd->a09,MSG_ERROR,"E0040: failed writing object file");
    else
      return message(opd->a09,MSG_ERROR,"E0041: truncated output to object file");
  }
  return true;
}

/**************************************************************************/

bool fdefault__opt(struct format *fmt,struct opcdata *opd,label *tmp)
{
  (void)fmt;
  (void)opd;
  (void)tmp;
  return true;
}

/**************************************************************************/

bool fdefault__test(struct format *fmt,struct opcdata *opd)
{
  (void)fmt;
  assert(opd      != NULL);
  assert(opd->a09 != NULL);
  assert((opd->pass == 1) || (opd->pass == 2));
  
  print_list(opd->a09,opd,false);
  
  while(!feof(opd->a09->in))
  {
    struct opcode const *op;
    label                label;
    char                 c;
    
    if (!read_line(opd->a09->in,&opd->a09->inbuf))
      return false;
      
    opd->a09->lnum++;
    print_list(opd->a09,opd,false); // XXX extra line in list file
    
    parse_label(&label,&opd->a09->inbuf,opd->a09,opd->pass);
    c = skip_space(&opd->a09->inbuf);
    if ((c == ';') || (c == '\0'))
      continue;
      
    opd->a09->inbuf.ridx--;
    if (!parse_op(&opd->a09->inbuf,&op))
      return message(opd->a09,MSG_ERROR,"E0003: unknown opcode");
    if (memcmp(op->name,".ENDTST",8) == 0)
      return true;
      
    //print_list(opd->a09,opd,false); // XXX missing line in list file
  }
  return message(opd->a09,MSG_ERROR,"E0010: unexpected end of input");
}

/**************************************************************************/

bool fdefault_fini(struct format *fmt,struct a09 *a09)
{
  assert(fmt != NULL);
  (void)a09;
  
  /*-----------------------------------------------------------------------
  ; The bin backend uses fmt->data as a flag, NOT a pointer, so we have to
  ; special case this here.  Sigh.
  ;------------------------------------------------------------------------*/
  
  if (fmt->data != NULL)
    if (fmt->data != fmt)
      free(fmt->data);
  return true;
}

/**************************************************************************/
