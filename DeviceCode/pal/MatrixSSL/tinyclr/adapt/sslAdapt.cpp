




//****************************************************************************
//
// Public API functions
//
//****************************************************************************

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
