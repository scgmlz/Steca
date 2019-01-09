!#############################################################################################
! Copyright (C) 2013 Antonio Cervellino
! 
! Permission is hereby granted, free of charge, to any person obtaining a copy of
! this software and associated documentation files (the "Software"), to deal in
! the Software without restriction, including without limitation the rights to
! use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
! the Software, and to permit persons to whom the Software is furnished to do so,
! subject to the following conditions:
! 
! The above copyright notice and this permission notice shall be included in all
! copies or substantial portions of the Software.
! 
! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
! FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
! COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
! IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
! CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
!
!  Authors:
!    Antonio Cervellino
!
!  E-mail:
!    antonio.cervellino@psi.ch
! 
!#############################################################################################
program callcerf
use use_libcerf
implicit none
real(DP) :: x,sigma, wid, y
complex(DPC) :: z, u

print*,'*********************************************************'
print*,'Calling Voigt_F'
print*,'Please give as input a real number x [abscissa], '
print*,' a real number sigma [Gaussian width],'
print*,' a real number wid [Lorentzian HWHM]:'
read(*,*) x, sigma, wid

y = Voigt_F( x, sigma, wid )

print*,'Voigt_F( ',x,', ',sigma,', ',wid,' ) = ',y

print*,'*********************************************************'
print*,'Calling erfcx_F'
print*,'Please give as input a real number x:'
read(*,*) x

y = Erfcx_F( x )

print*,'erfcx_F( ',x,' ) = ',y

print*,'*********************************************************'
print*,'Calling erfi_F'
print*,'Please give as input a real number x:'
read(*,*) x

y = Erfi_F( x )

print*,'erfi_F( ',x,' ) = ',y

print*,'*********************************************************'
print*,'Calling im_w_of_x_F'
print*,'Please give as input a real number x:'
read(*,*) x

y = Im_w_of_x_F( x )

print*,'im_w_of_x_F( ',x,' ) = ',y

print*,'*********************************************************'
print*,'Calling Dawson_F'
print*,'Please give as input a real number x:'
read(*,*) x

y = Dawson_F( x )

print*,'Dawson_F( ',x,' ) = ',y



print*,'*********************************************************'
print*,'Calling cerf_F'
print*,'Please give as input a complex number number z,'
print*,' as two space-separated real numbers Re(z), Im(z) :'
read(*,*) x,y
z=CMPLX(x,y,DPC)

u = cerf_F( z )

print*,'cerf_F( ',z,' ) = ',u

print*,'*********************************************************'
print*,'Calling cerfc_F'
print*,'Please give as input a complex number number z,'
print*,' as two space-separated real numbers Re(z), Im(z) :'
read(*,*) x,y
z=CMPLX(x,y,DPC)

u = cerfc_F( z )

print*,'cerfc_F( ',z,' ) = ',u

print*,'*********************************************************'
print*,'Calling cerfcx_F'
print*,'Please give as input a complex number number z,'
print*,' as two space-separated real numbers Re(z), Im(z) :'
read(*,*) x,y
z=CMPLX(x,y,DPC)

u = cerfcx_F( z )

print*,'cerfcx_F( ',z,' ) = ',u

print*,'*********************************************************'
print*,'Calling cerfi_F'
print*,'Please give as input a complex number number z,'
print*,' as two space-separated real numbers Re(z), Im(z) :'
read(*,*) x,y
z=CMPLX(x,y,DPC)

u = cerfi_F( z )

print*,'cerfi_F( ',z,' ) = ',u

print*,'*********************************************************'
print*,'Calling w_of_z_F'
print*,'Please give as input a complex number number z,'
print*,' as two space-separated real numbers Re(z), Im(z) :'
read(*,*) x,y
z=CMPLX(x,y,DPC)

u = w_of_z_F( z )

print*,'w_of_z_F( ',z,' ) = ',u

print*,'*********************************************************'
print*,'Calling cdawson_F'
print*,'Please give as input a complex number number z,'
print*,' as two space-separated real numbers Re(z), Im(z) :'
read(*,*) x,y
z=CMPLX(x,y,DPC)

u = cdawson_F( z )

print*,'cdawson_F( ',z,' ) = ',u

end program callcerf