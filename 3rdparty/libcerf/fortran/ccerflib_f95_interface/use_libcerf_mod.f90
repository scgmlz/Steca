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
module use_libcerf
! f95 interfaces for the libcerf.* c library of error functions
! http://apps.jcns.fz-juelich.de/libcerf
! 
use ISO_C_BINDING 

private
public :: Voigt_F, Erfcx_F, Erfi_F, Im_w_of_x_F, Dawson_F, &
          cerf_F, cerfc_F, cerfcx_F, cerfi_F, w_of_z_F, cdawson_F
public :: DP,DPC
! - - - - - - - - - - - - 

! Fortran DOUBLE and DOUBLE COMPLEX types
INTEGER, PARAMETER :: DP  = KIND(1.0D0)
INTEGER, PARAMETER :: DPC = KIND((1.0D0,1.0D0))

! - - - - - - - - - - - - 
INTERFACE
FUNCTION voigt ( x, sigma, gamma ) bind(c)
USE ISO_C_BINDING
IMPLICIT NONE
! voigt(x,sigma,gamma) = \int_|R G(t,sigma) L(x-t,gamma) dt
! WITH: G(x,sigma) = (1/(sqrt(2*pi)*|sigma|)) * exp(-x**2/(2*sigma**2)) ;
!       L(x,gamma) = |gamma| / (pi * ( x**2 + gamma**2 )) .
real(C_DOUBLE),intent(IN),VALUE :: x, sigma, gamma
real(C_DOUBLE) :: voigt
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION cerf ( z ) bind(c)
!  cerf(z) = erf(z)
USE ISO_C_BINDING
IMPLICIT NONE
complex(C_DOUBLE_COMPLEX),intent(IN),VALUE :: z
complex(C_DOUBLE_COMPLEX) :: cerf
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION cerfc ( z ) bind(c)
!  cerfc(z) = erfc(z)
USE ISO_C_BINDING
IMPLICIT NONE
complex(C_DOUBLE_COMPLEX),intent(IN),VALUE :: z
complex(C_DOUBLE_COMPLEX) :: cerfc
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION cerfcx ( z ) bind(c)
USE ISO_C_BINDING
IMPLICIT NONE
!  cerfcx(z) = exp(z**2) * erfc(z)
complex(C_DOUBLE_COMPLEX),intent(IN),VALUE :: z
complex(C_DOUBLE_COMPLEX) :: cerfcx
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION erfcx ( x ) bind(c)
USE ISO_C_BINDING
IMPLICIT NONE
!  erfcx(x) = exp(x**2) * erfc(x)
real(C_DOUBLE),intent(IN),VALUE :: x
real(C_DOUBLE) :: erfcx
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION cerfi ( z ) bind(c)
USE ISO_C_BINDING
IMPLICIT NONE
!  cerfi(z) = -I * erf(I*z)
complex(C_DOUBLE_COMPLEX),intent(IN),VALUE :: z
complex(C_DOUBLE_COMPLEX) :: cerfi
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION erfi ( x ) bind(c)
USE ISO_C_BINDING
IMPLICIT NONE
!  erfi(x) = -I * erf(I*x)
real(C_DOUBLE),intent(IN),VALUE :: x
real(C_DOUBLE) :: erfi
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION w_of_z ( z ) bind(c)
USE ISO_C_BINDING
IMPLICIT NONE
!  w_of_z(z) = [ Faddeeva function w(z) ] 
!            = exp(-z**2) * erfc(-I*z)
complex(C_DOUBLE_COMPLEX),intent(IN),VALUE :: z
complex(C_DOUBLE_COMPLEX) :: w_of_z
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION im_w_of_x ( x ) bind(c)
USE ISO_C_BINDING
IMPLICIT NONE
!  Im_w_of_x(x) = Im[w(x)]
real(C_DOUBLE),intent(IN),VALUE :: x
real(C_DOUBLE) :: im_w_of_x
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION cdawson ( z ) bind(c)
USE ISO_C_BINDING
IMPLICIT NONE
!  cdawson(z) = [ Dawson's integral D(z) ] 
!             = exp(-z**2) \int_0^z exp(t**2) dt 
!             = sqrt(pi)/2 * exp(-z**2) * erfi(z)
complex(C_DOUBLE_COMPLEX),intent(IN),VALUE :: z
complex(C_DOUBLE_COMPLEX) :: cdawson
END FUNCTION
END INTERFACE
! - - - - - - - - - - - - 
INTERFACE
FUNCTION dawson ( x ) bind(c)
USE ISO_C_BINDING
IMPLICIT NONE
!  dawson(x) = [ Dawson's integral D(x) ] 
!            = exp(-x**2) \int_0^x exp(t**2) dt 
!            = sqrt(pi)/2 * exp(-x**2) * erfi(x)
real(C_DOUBLE),intent(IN),VALUE :: x
real(C_DOUBLE) :: dawson
END FUNCTION
END INTERFACE

contains
!********************************************************
function Voigt_F(x,sig,w) !bind(c)
! voigt(x,sigma,gamma) = \int_|R G(t,sigma) L(x-t,gamma) dt
! WITH: G(x,sigma) = (1/(sqrt(2*pi)*|sigma|)) * exp(-x**2/(2*sigma**2)) ;
!       L(x,gamma) = |gamma| / (pi * ( x**2 + gamma**2 )) .
implicit none
real(DP),intent(IN) :: x,sig,w
real(DP) :: Voigt_F

Voigt_F = real(voigt( x=real(x,C_DOUBLE), sigma=real(sig,C_DOUBLE), gamma=real(w,C_DOUBLE) ), DP)

end function Voigt_F
!********************************************************
function Erfcx_F(x) !bind(c)
!  erfcx(x) = exp(x**2) * erfc(x)
implicit none
real(DP),intent(IN) :: x
real(DP) :: Erfcx_F

Erfcx_F = real(erfcx( x=real(x,C_DOUBLE) ), DP)

end function Erfcx_F
!********************************************************
function Erfi_F(x) !bind(c)
!  erfi(x) = -I * erf(I*x)
implicit none
real(DP),intent(IN) :: x
real(DP) :: Erfi_F

Erfi_F = real(erfi( x=real(x,C_DOUBLE) ), DP)

end function Erfi_F
!********************************************************
function Im_w_of_x_F(x) !bind(c)
!  Im_w_of_x(x) = Im[w(x)]
implicit none
real(DP),intent(IN) :: x
real(DP) :: Im_w_of_x_F

Im_w_of_x_F = real(im_w_of_x( x=real(x,C_DOUBLE) ), DP)

end function Im_w_of_x_F
!********************************************************
function Dawson_F(x) !bind(c)
!  dawson(x) = [ Dawson's integral D(x) ] 
!            = exp(-x**2) \int_0^x exp(t**2) dt 
!            = sqrt(pi)/2 * exp(-x**2) * erfi(x)
implicit none
real(DP),intent(IN) :: x
real(DP) :: Dawson_F

Dawson_F = real(dawson( x=real(x,C_DOUBLE) ), DP)

end function Dawson_F
!********************************************************
! Complex
!********************************************************
function cerf_F ( z ) !bind(c)
!  cerf(z) = erf(z)
implicit none
complex(DPC),intent(IN) :: z
complex(DPC) :: cerf_F

cerf_F = CMPLX(cerf( z=CMPLX(z,kind=C_DOUBLE_COMPLEX) ), kind=DPC)

end function cerf_F
!********************************************************
function cerfc_F ( z ) !bind(c)
!  cerfc(z) = erfc(z)
implicit none
complex(DPC),intent(IN) :: z
complex(DPC) :: cerfc_F

cerfc_F = CMPLX(cerfc( z=CMPLX(z,kind=C_DOUBLE_COMPLEX) ), kind=DPC)

end function cerfc_F
!********************************************************
function cerfcx_F ( z ) !bind(c)
implicit none
!  cerfcx(z) = exp(z**2) * erfc(z)
complex(DPC),intent(IN) :: z
complex(DPC) :: cerfcx_F

cerfcx_F = CMPLX(cerfcx( z=CMPLX(z,kind=C_DOUBLE_COMPLEX) ), kind=DPC)

end function cerfcx_F
!********************************************************
function cerfi_F ( z ) !bind(c)
implicit none
!  cerfi(z) = -I * erf(I*z)
complex(DPC),intent(IN) :: z
complex(DPC) :: cerfi_F

cerfi_F = CMPLX(cerfi( z=CMPLX(z,kind=C_DOUBLE_COMPLEX) ), kind=DPC)

end function cerfi_F
!********************************************************
function w_of_z_F ( z ) !bind(c)
implicit none
!  w_of_z(z) = [ Faddeeva function w(z) ] 
!            = exp(-z**2) * erfc(-I*z)
complex(DPC),intent(IN) :: z
complex(DPC) :: w_of_z_F

w_of_z_F = CMPLX(w_of_z( z=CMPLX(z,kind=C_DOUBLE_COMPLEX) ), kind=DPC)

end function w_of_z_F
!********************************************************
function cdawson_F ( z ) !bind(c)
implicit none
!  cdawson(z) = [ Dawson's integral D(z) ] 
!             = exp(-z**2) \int_0^z exp(t**2) dt 
!             = sqrt(pi)/2 * exp(-z**2) * erfi(z)
complex(DPC),intent(IN) :: z
complex(DPC) :: cdawson_F

cdawson_F = CMPLX(cdawson( z=CMPLX(z,kind=C_DOUBLE_COMPLEX) ), kind=DPC)

end function cdawson_F
!********************************************************
end module use_libcerf
