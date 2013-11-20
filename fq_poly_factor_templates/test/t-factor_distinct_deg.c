/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2013 Mike Hansen

******************************************************************************/


#ifdef T

#include "templates.h"

#include <stdlib.h>
#include "ulong_extras.h"
int
main(void)
{
    int iter;
    flint_rand_t state;
    flint_randinit(state);

    flint_printf("factor_distinct_deg....");
    fflush(stdout);

    for (iter = 0; iter < 200; iter++)
    {
        TEMPLATE(T, ctx_t) ctx;
        TEMPLATE(T, poly_t) poly1, poly, q, r, product;
        TEMPLATE(T, poly_factor_t) res;
        slong i, length, num;
        slong *degs;

        TEMPLATE(T, ctx_randtest)(ctx, state);

        TEMPLATE(T, poly_init)(poly1, ctx);
        TEMPLATE(T, poly_init)(poly, ctx);
        TEMPLATE(T, poly_init)(q, ctx);
        TEMPLATE(T, poly_init)(r, ctx);

        TEMPLATE(T, poly_zero)(poly1, ctx);
        TEMPLATE(T, poly_one)(poly1, ctx);

        length = n_randint(state, 7) + 2;
        do
        {
            TEMPLATE(T, poly_randtest)(poly, state, length, ctx);
            if (poly->length)
                TEMPLATE(T, poly_make_monic)(poly, poly, ctx);
        }
        while ((poly->length < 2) || (!TEMPLATE(T, poly_is_irreducible)(poly, ctx)));

        TEMPLATE(T, poly_mul)(poly1, poly1, poly, ctx);

        num = n_randint(state, 5) + 1;

        for (i = 1; i < num; i++)
        {
            do
            {
                length = n_randint(state, 7) + 2;
                TEMPLATE(T, poly_randtest)(poly, state, length, ctx);
                if (poly->length)
                {
                    TEMPLATE(T, poly_make_monic)(poly, poly, ctx);
                    TEMPLATE(T, poly_divrem)(q, r, poly1, poly, ctx);
                }
            }
            while ((poly->length < 2) || (!TEMPLATE(T, poly_is_irreducible)(poly, ctx))
                   || (r->length == 0));

            TEMPLATE(T, poly_mul)(poly1, poly1, poly, ctx);
        }

        if (!(degs = flint_malloc((poly1->length - 1) * sizeof(slong))))
        {
            flint_printf("Fatal error: not enough memory.");
            abort();
        }
        TEMPLATE(T, poly_factor_init)(res, ctx);
        TEMPLATE(T, poly_factor_distinct_deg)(res, poly1, &degs, ctx);

        TEMPLATE(T, poly_init)(product, ctx);
        TEMPLATE(T, poly_one)(product, ctx);
        for (i = 0; i < res->num; i++)
            TEMPLATE(T, poly_mul)(product, product, res->poly + i, ctx);

        TEMPLATE(T, TEMPLATE(poly_scalar_mul, T))(product, product,
                              poly1->coeffs + (poly1->length - 1), ctx);

        if (!TEMPLATE(T, poly_equal)(poly1, product, ctx))
        {
            flint_printf
                ("Error: product of factors does not equal to the original polynomial\n");
            flint_printf("poly:\n");
            TEMPLATE(T, poly_print)(poly1, ctx);
            flint_printf("\n");
            flint_printf("product:\n");
            TEMPLATE(T, poly_print)(product, ctx);
            flint_printf("\n");
            abort();
        }

        flint_free(degs);
        TEMPLATE(T, poly_clear)(product, ctx);
        TEMPLATE(T, poly_clear)(q, ctx);
        TEMPLATE(T, poly_clear)(r, ctx);
        TEMPLATE(T, poly_clear)(poly1, ctx);
        TEMPLATE(T, poly_clear)(poly, ctx);
        TEMPLATE(T, poly_factor_clear)(res, ctx);

        TEMPLATE(T, ctx_clear)(ctx);
    }

    flint_randclear(state);
    _fmpz_cleanup();
    flint_printf("PASS\n");
    return 0;
}


#endif