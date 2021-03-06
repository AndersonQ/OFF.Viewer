/*
 * This file is part of OFFViewer.
 *
 * OFFViewer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OFFViewer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OFFViewer.  If not, see <http://www.gnu.org/licenses/>.
 */

attribute vec4 vPosition;
attribute vec4 vColour;
uniform mat4 MatrixModelView;
uniform mat4 MatrixProjection;
uniform mat4 MatrixRotation;

varying vec4 color;

void main()
{
    gl_Position = MatrixProjection * MatrixModelView * MatrixRotation * vPosition;
    color = vColour;
}
