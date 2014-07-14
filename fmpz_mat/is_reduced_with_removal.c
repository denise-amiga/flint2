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

    Copyright (C) 2014 Abhinav Baid

******************************************************************************/

#include "fmpz_mat.h"
#include "fmpq_vec.h"
#include "fmpq_mat.h"

int
fmpz_mat_is_reduced_with_removal(const fmpz_mat_t A, double delta, double eta,
                                 const fmpz_t gs_B, int newd)
{
    slong i, j, k, d = A->r, n = A->c;
    fmpq_mat_t Aq, Bq, mu;
    fmpq_lll_t fl;
    fmpq_t tmp, gs_Bq;

    if (d == 0 || d == 1)
        return 1;

    fmpq_mat_init(Aq, d, n);
    fmpq_mat_init(Bq, d, n);
    fmpq_mat_init(mu, d, d);

    fmpq_lll_context_init(fl, delta, eta);

    fmpq_init(tmp);
    fmpq_init(gs_Bq);

    fmpq_mat_set_fmpz_mat(Aq, A);

    fmpz_set(fmpq_numref(gs_Bq), gs_B);
    fmpz_one(fmpq_denref(gs_Bq));

    for (j = 0; j < n; j++)
    {
        fmpq_set(fmpq_mat_entry(Bq, 0, j), fmpq_mat_entry(Aq, 0, j));
    }
    /* diagonal of mu stores the squared GS norms */
    _fmpq_vec_dot(fmpq_mat_entry(mu, 0, 0), Bq->rows[0], Bq->rows[0], n);
    if (newd == 0 && fmpq_cmp(fmpq_mat_entry(mu, 0, 0), gs_Bq) < 0)
    {
        fmpq_mat_clear(Aq);
        fmpq_mat_clear(Bq);
        fmpq_mat_clear(mu);
        fmpq_lll_context_clear(fl);
        fmpq_clear(tmp);
        fmpq_clear(gs_Bq);
        return 0;
    }

    for (i = 1; i < d; i++)
    {
        for (j = 0; j < n; j++)
        {
            fmpq_set(fmpq_mat_entry(Bq, i, j), fmpq_mat_entry(Aq, i, j));
        }

        for (j = 0; j < i; j++)
        {
            _fmpq_vec_dot(tmp, Aq->rows[i], Bq->rows[j], n);

            fmpq_div(fmpq_mat_entry(mu, i, j), tmp, fmpq_mat_entry(mu, j, j));

            for (k = 0; k < n; k++)
            {
                fmpq_submul(fmpq_mat_entry(Bq, i, k),
                            fmpq_mat_entry(mu, i, j), fmpq_mat_entry(Bq, j,
                                                                     k));
            }
            if (i < newd)
            {
                fmpq_abs(tmp, fmpq_mat_entry(mu, i, j));
                if (fmpq_cmp(tmp, fl->eta) > 0) /* check size reduction */
                {
                    fmpq_mat_clear(Aq);
                    fmpq_mat_clear(Bq);
                    fmpq_mat_clear(mu);
                    fmpq_lll_context_clear(fl);
                    fmpq_clear(tmp);
                    fmpq_clear(gs_Bq);
                    return 0;
                }
            }
        }
        _fmpq_vec_dot(fmpq_mat_entry(mu, i, i), Bq->rows[i], Bq->rows[i], n);
        if (i >= newd && fmpq_cmp(fmpq_mat_entry(mu, i, i), gs_Bq) < 0) /* check removals */
        {
            fmpq_mat_clear(Aq);
            fmpq_mat_clear(Bq);
            fmpq_mat_clear(mu);
            fmpq_lll_context_clear(fl);
            fmpq_clear(tmp);
            fmpq_clear(gs_Bq);
            return 0;
        }
        if (i < newd)
        {
            fmpq_set(tmp, fl->delta);
            fmpq_submul(tmp, fmpq_mat_entry(mu, i, i - 1),
                        fmpq_mat_entry(mu, i, i - 1));
            fmpq_mul(tmp, tmp, fmpq_mat_entry(mu, i - 1, i - 1));
            if (fmpq_cmp(tmp, fmpq_mat_entry(mu, i, i)) > 0)    /* check Lovasz condition */
            {
                fmpq_mat_clear(Aq);
                fmpq_mat_clear(Bq);
                fmpq_mat_clear(mu);
                fmpq_lll_context_clear(fl);
                fmpq_clear(tmp);
                fmpq_clear(gs_Bq);
                return 0;
            }
        }
    }

    fmpq_mat_clear(Aq);
    fmpq_mat_clear(Bq);
    fmpq_mat_clear(mu);
    fmpq_lll_context_clear(fl);
    fmpq_clear(tmp);
    fmpq_clear(gs_Bq);
    return 1;
}
