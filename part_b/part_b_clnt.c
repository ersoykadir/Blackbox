/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "part_b.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

outputData *
part_b_1(inputData *argp, CLIENT *clnt)
{
	static outputData clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, part_b,
		(xdrproc_t) xdr_inputData, (caddr_t) argp,
		(xdrproc_t) xdr_outputData, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}