/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                                   Extrae                                  *
 *              Instrumentation package for parallel applications            *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- *\
 | @file: $HeadURL: https://svn.bsc.es/repos/ptools/extrae/tags/3.2/3.2.1/example/LINUX/FORK/pi_forked.c $
 | @last_commit: $Date: 2013-02-26 18:43:31 +0100 (Tue, 26 Feb 2013) $
 | @version:     $Revision: 1552 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

double pi_kernel (int n, double h)
{
	double tmp = 0;
	double x;
	int i;

	for (i = 1; i <= n; i++)
	{
		x = h * ((double)i - 0.5);
		tmp += (4.0 / (1.0 + x*x));
	}

	return tmp;
}

int main(int argc, char **argv)
{
	int n = 100*1000*1000;
	double PI25DT = 3.141592653589793238462643;
	double pi, h, area, x;
	pid_t p;
	int i;

	p = fork();

	if (p == 0)
	{
		printf ("Child is about to calculate pi...\n");
		fflush (stdout);
		pi_kernel (n, h);
		printf ("Child finished\n");
		fflush (stdout);
	}
	else
	{
		int status;
		printf ("Parent is waiting...\n");
		fflush (stdout);
		waitpid (p, &status, 0);
		printf ("Parent finished waiting\n");
		fflush (stdout);
	}
	return 0;
}
