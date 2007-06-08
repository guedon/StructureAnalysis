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


#include "rw_date.h"
#include "rw_time.h"

#ifdef RWOUT

TOOLS_BEGIN_NAMESPACE

CDate::CDate( unsigned day, unsigned month, unsigned year )
{
    struct tm* timeinfo;
    s= time(0);
    timeinfo= localtime(&s);
    timeinfo->tm_mday= day;
    timeinfo->tm_mon= month - 1;
	if(year > 1900)timeinfo->tm_year= year - 1900;
    else if (year < 50) timeinfo->tm_year= year + 100;
	else timeinfo->tm_year= year;
    s= mktime(timeinfo);
}

CDate::CDate( const Time& t ) { s= t.s; }

void CDate::extract( struct tm* t ) const
{
    struct tm* timeinfo= localtime(&s);
	*t = *timeinfo;
}

TOOLS_END_NAMESPACE

#endif
