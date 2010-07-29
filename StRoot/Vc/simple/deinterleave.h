/*  This file is part of the Vc library.

    Copyright (C) 2010 Matthias Kretz <kretz@kde.org>

    Vc is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    Vc is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Vc.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef VC_SCALAR_DEINTERLEAVE_H
#define VC_SCALAR_DEINTERLEAVE_H

namespace Vc
{
namespace Internal
{

template<> struct HelperImpl<Vc::ScalarImpl>
{
    template<typename V, typename M, typename A>
    static inline void deinterleave(V &a, V &b, const M *mem, A)
    {
        a = mem[0];
        b = mem[1];
    }
};

} // namespace Scalar
} // namespace Vc

#endif // VC_SCALAR_DEINTERLEAVE_H
