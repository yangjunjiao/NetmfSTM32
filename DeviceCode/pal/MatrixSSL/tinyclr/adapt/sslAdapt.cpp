




//****************************************************************************
//
// Public API functions
//
//****************************************************************************


const char* c_returnValuesError[] =
{
"PS_ARG_FAIL", //			-6	/* Failure due to bad function param */
"PS_PLATFORM_FAIL", //	-7	/* Failure as a result of system call error */
"PS_MEM_FAIL", //			-8	/* Failure to allocate requested memory */
"PS_LIMIT_FAIL", //		-9	/* Failure on sanity/limit tests */
"PS_UNSUPPORTED_FAIL", //	-10 /* Unimplemented feature error */
"PS_DISABLED_FEATURE_FAIL", // -11 /* Incorrect #define toggle for feature */
"PS_PROTOCOL_FAIL", //	-12 /* A protocol error occurred */
"PS_TIMEOUT_FAIL", //		-13 /* A timeout occurred and MAY be an error */
"PS_INTERRUPT_FAIL", //	-14 /* An interrupt occurred and MAY be an error */
"PS_PENDING" //			-15 /* In process. Not necessarily an error */
};


const char* c_returnValues[] =
                           {
                           		"MATRIXSSL_ERROR", //actually -1
                           		"MATRIXSSL_SUCCESS",
                           		"MATRIXSSL_REQUEST_SEND",
                           		"MATRIXSSL_REQUEST_RECV",
                           		"MATRIXSSL_REQUEST_CLOSE",
                           		"MATRIXSSL_APP_DATA",
                           		"MATRIXSSL_HANDSHAKE_COMPLETE",
                           		"MATRIXSSL_RECEIVED_ALERT"
                           };

const char * GetReturnValues(int returnValue)
{
	if(returnValue >=-1 && returnValue < 8)
		return c_returnValues[returnValue + 1];
	if(returnValue >=-15 && returnValue < -6)
		return c_returnValuesError[returnValue + 18];
	return "RETURN VALUE NOT DEFINED";
}

/*
err_t
ssl_init(void)
{
    int iRet;

    DEBUG_MSG("ssl_init\n");

    //
    // Initialize the SSL library.
    //
    iRet = matrixSslOpen();
    if(iRet < 0)
    {
        //
        // Something went wrong so kill the application with an error code.
        //
        return(ERR_IF);
    }

    //
    // Read the required certificates from memory.
    //
    iRet = matrixSslReadKeysMem(&g_pSslKeys, g_pcCertSrv, g_ulCertSrvLen,
                                g_pcPrivKeySrv, g_ulPrivKeySrvLen, NULL, 0);
    if(iRet < 0)
    {
        //
        // Something went wrong so kill the application with an error code.
        //
        return(ERR_MEM);
    }

    return(ERR_OK);
}*/
