#pragma once

#include <plotting/types/point_types.h>

namespace plotting
{
    namespace algorithms
    {
        void Clip1Left(repr_t& A, repr_t& B, repr_t const& Pmin, repr_t const& /*Pmax*/)
        {
            A.y = B.y - (B.x - Pmin.x) * (B.y - A.y) / (B.x - A.x);
            A.x = Pmin.x;
        }

        void Clip1Top(repr_t& A, repr_t& B, repr_t const& /*Pmin*/, repr_t const& Pmax)
        {
            A.x = B.x - (B.y - Pmax.y) * (B.x - A.x) / (B.y - A.y);
            A.y = Pmax.y;
        }

        void Clip1Bottom(repr_t& A, repr_t& B, repr_t const& Pmin, repr_t const& /*Pmax*/)
        {
            A.x = B.x - (B.y - Pmin.y) * (B.x - A.x) / (B.y - A.y);
            A.y = Pmin.y;
        }

        void Clip2Right(repr_t A, repr_t& B, repr_t const& /*Pmin*/, repr_t const& Pmax)
        {
            B.y = A.y + (Pmax.x - A.x) * (B.y - A.y) / (B.x - A.x);
            B.x = Pmax.x;
        }

        void Clip2Top(repr_t A, repr_t& B, repr_t const& /*Pmin*/, repr_t const& Pmax)
        {
            B.x = A.x + (Pmax.y - A.y) * (B.x - A.x) / (B.y - A.y);
            B.y = Pmax.y;
        }

        void Clip2Bottom(repr_t A, repr_t& B, repr_t const& Pmin, repr_t const& /*Pmax*/)
        {
            B.x = A.x + (Pmin.y - A.y) * (B.x - A.x) / (B.y - A.y);
            B.y = Pmin.y;
        }

        // FC-Algorithm
        bool clip(repr_t& A, repr_t& B, repr_t Pmin, repr_t Pmax)
        {
            // 1
            if(A.x > B.x) std::swap(A, B);

            // 2
            int C1 = 0;
            //if(A.x < Pmin.x) C1 += 1;
            //if(A.x > Pmax.x) C1 += 2;
            //if(A.y < Pmin.y) C1 += 4;
            //if(A.y > Pmax.y) C1 += 8;

            C1 |= (A.x < Pmin.x);
            C1 |= (A.x > Pmax.x)<<1;
            C1 |= (A.y < Pmin.y)<<2;
            C1 |= (A.y > Pmax.y)<<3;


            int C2 = 0;
            //if(B.x < Pmin.x) C2 += 1;
            //if(B.x > Pmax.x) C2 += 2;
            //if(B.y < Pmin.y) C2 += 4;
            //if(B.y > Pmax.y) C2 += 8;

            C2 |= (B.x < Pmin.x);
            C2 |= (B.x > Pmax.x)<<1;
            C2 |= (B.y < Pmin.y)<<2;
            C2 |= (B.y > Pmax.y)<<3;

            // 3
            if(C1 & C2) return false;

            // 4
            int C = C1 * 16 + C2;

            switch(C) {
            // 5
            default:
            /*case 0x00:*/ return true;

            // 6
            case 0x02: {
                Clip2Right(A, B, Pmin, Pmax);
                return true;
            }

            // 7
            case 0x04: {
                Clip2Bottom(A, B, Pmin, Pmax);
                return true;
            }

            // 8
            case 0x06: {
                Clip2Right(A, B, Pmin, Pmax);

                if(B.y < Pmin.y) Clip2Bottom(A, B, Pmin, Pmax);
                return true;
            }

            // 9
            case 0x08: {
                Clip2Top(A, B, Pmin, Pmax);
                return true;
            }

            // 10
            case 0x0A: {
                Clip2Right(A, B, Pmin, Pmax);

                if(B.y > Pmax.y) Clip2Top(A, B, Pmin, Pmax);
                return true;
            }

            // 11
            case 0x10: {
                Clip1Left(A, B, Pmin, Pmax);
                return true;
            }

            // 12
            case 0x12: {
                Clip1Left(A, B, Pmin, Pmax);
                Clip2Right(A, B, Pmin, Pmax);
                return true;
            }

            // 13
            case 0x14: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y < Pmin.y) return false;
                else Clip2Bottom(A, B, Pmin, Pmax);
                return true;
            }

            // 14
            case 0x16: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y < Pmin.y) return false;
                else Clip2Bottom(A, B, Pmin, Pmax);

                if(B.x > Pmax.x) Clip2Right(A, B, Pmin, Pmax);
                return true;
            }

            // 15
            case 0x18: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y > Pmax.y) return false;
                else Clip2Top(A, B, Pmin, Pmax);
                return true;
            }

            // 16
            case 0x1A: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y > Pmax.y) return false;
                else Clip2Top(A, B, Pmin, Pmax);

                if(B.x > Pmax.x) Clip2Right(A, B, Pmin, Pmax);
                return true;
            }

            // 17
            case 0x40: {
                Clip1Bottom(A, B, Pmin, Pmax);
                return true;
            }

            // 18
            case 0x42: {
                Clip1Bottom(A, B, Pmin, Pmax);

                if(A.x > Pmax.x) return false;
                else Clip2Right(A, B, Pmin, Pmax);
                return true;
            }

            // 19
            case 0x48: {
                Clip1Bottom(A, B, Pmin, Pmax);
                Clip2Top(A, B, Pmin, Pmax);
                return true;
            }

            // 20
            case 0x4A: {
                Clip1Bottom(A, B, Pmin, Pmax);

                if(A.x > Pmax.x) return false;
                else Clip2Right(A, B, Pmin, Pmax);

                if(B.y > Pmax.y) Clip2Top(A, B, Pmin, Pmax);

                return true;
            }

            // 21
            case 0x50: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y < Pmin.y) Clip1Bottom(A, B, Pmin, Pmax);

                return true;
            }

            // 22
            case 0x52: {
                Clip1Bottom(A, B, Pmin, Pmax);

                if(A.x > Pmax.x) return false;
                else if(A.x < Pmin.x) Clip1Left(A, B, Pmin, Pmax);

                Clip2Right(A, B, Pmin, Pmax);
                return true;
            }

            // 23
            case 0x58: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y > Pmax.y) return false;
                else if(A.y < Pmin.y) Clip1Bottom(A, B, Pmin, Pmax);

                Clip2Top(A, B, Pmin, Pmax);
                return true;
            }

            // 24
            case 0x5A: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y > Pmax.y) return false;
                else if(A.y < Pmin.y) {
                    Clip1Bottom(A, B, Pmin, Pmax);
                    if(A.x > Pmax.x) return false;
                }

                Clip2Top(A, B, Pmin, Pmax);

                if(B.x > Pmax.x) Clip2Right(A, B, Pmin, Pmax);
                return true;
            }

            // 25
            case 0x80: {
                Clip1Top(A, B, Pmin, Pmax);
                return true;
            }

            // 26
            case 0x82: {
                Clip1Top(A, B, Pmin, Pmax);

                if(A.x > Pmax.x) return false;
                else Clip2Right(A, B, Pmin, Pmax);
                return true;
            }

            // 27
            case 0x84: {
                Clip1Top(A, B, Pmin, Pmax);
                Clip2Bottom(A, B, Pmin, Pmax);
                return true;
            }

            // 28
            case 0x86: {
                Clip1Top(A, B, Pmin, Pmax);

                if(A.x > Pmax.x) return false;
                else Clip2Right(A, B, Pmin, Pmax);

                if(B.y < Pmin.y) Clip2Bottom(A, B, Pmin, Pmax);
                return true;
            }

            // 29
            case 0x90: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y > Pmax.y) Clip1Top(A, B, Pmin, Pmax);
                return true;
            }

            // 30
            case 0x92: {
                Clip1Top(A, B, Pmin, Pmax);

                if(A.x > Pmax.x) return false;
                else if(A.x < Pmin.x) Clip1Left(A, B, Pmin, Pmax);

                Clip2Right(A, B, Pmin, Pmax);
                return true;
            }

            // 31
            case 0x94: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y < Pmin.y) return false;
                else if(A.y > Pmax.y) Clip1Top(A, B, Pmin, Pmax);

                Clip2Bottom(A, B, Pmin, Pmax);
                return true;
            }

            // 32
            case 0x96: {
                Clip1Left(A, B, Pmin, Pmax);

                if(A.y < Pmin.y) return false;
                else if(A.y > Pmax.y) {
                    Clip1Top(A, B, Pmin, Pmax);
                    if(A.x > Pmax.x) return false;
                }

                Clip2Bottom(A, B, Pmin, Pmax);

                if(B.x > Pmax.x) Clip2Right(A, B, Pmin, Pmax);
                return true;
            }
            }
        }
    }
}