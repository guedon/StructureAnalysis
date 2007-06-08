/* -*-c++-*-
 *  ----------------------------------------------------------------------------
 *
 *       AMAPmod: Exploring and Modeling Plant Architecture
 *
 *       Copyright 1995-2002 UMR Cirad/Inra Modelisation des Plantes
 *
 *       File author(s): Ch. Pradal (christophe.pradal@cirad.fr)
 *
 *       $Source$
 *       $Id$
 *
 *       Forum for AMAPmod developers    : amldevlp@cirad.fr
 *
 *  ----------------------------------------------------------------------------
 *
 *                      GNU General Public Licence
 *
 *       This program is free software; you can redistribute it and/or
 *       modify it under the terms of the GNU General Public License as
 *       published by the Free Software Foundation; either version 2 of
 *       the License, or (at your option) any later version.
 *
 *       This program is distributed in the hope that it will be useful,
 *       but WITHOUT ANY WARRANTY; without even the implied warranty of
 *       MERCHANTABILITY or FITNESS For A PARTICULAR PURPOSE. See the
 *       GNU General Public License for more details.
 *
 *       You should have received a copy of the GNU General Public
 *       License along with this program; see the file COPYING. If not,
 *       write to the Free Software Foundation, Inc., 59
 *       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ----------------------------------------------------------------------------
 */

#ifndef __rw_defs_h__
#define __rw_defs_h__

/*! \file rw_defs.h
    \brief File for Rogue Wave Definition with the STL.
*/

#include "config.h"

#ifdef RWOUT

/* ----------------------------------------------------------------------- */


#include <string>

/* ----------------------------------------------------------------------- */

#define RW_NPOS std::string::npos
#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif
const size_t RWDEFAULT_CAPACITY = 64;
typedef bool RWBoolean;
typedef void RWCollectable;

/* ----------------------------------------------------------------------- */


#endif
// rw_defs_h
#endif
