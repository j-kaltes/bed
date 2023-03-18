/*     Bed a menu-driven multi dataformat binary editor for Linux            */
/*     Copyright (C) 1998 Jaap Korthals Altes <binaryeditor@gmx.com>       */
/*                                                                           */
/*     This program is free software; you can redistribute it and/or modify  */
/*     it under the terms of the GNU General Public License as published by  */
/*     the Free Software Foundation; either version 2 of the License, or     */
/*     (at your option) any later version.                                   */
/*                                                                           */
/*     This program is distributed in the hope that it will be useful,       */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */
/*                                                                           */
/*     You should have received a copy of the GNU General Public License     */
/*     along with this program; if not, write to the Free Software           */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             */
/* Fri Dec  8 22:14:27 2000                                                  */

int leftvis(ScreenPart *part) {return (part->editor->visualoffset-part->left);}
int rightvis(ScreenPart *part) {
	return (part->editor->visualwidth+part->editor->visualoffset-part->left);
	}
#define offvis(part,xpos) ((xpos)<leftvis(part)||(xpos)>=rightvis(part))
//#define offvis(part,xpos) (((xpos+part->left)<part->editor->visualoffset)||((xpos+part->left)>=(part->editor->visualwidth+part->editor->visualoffset)))
#define offvisual(xpos) offvis(this,xpos)

