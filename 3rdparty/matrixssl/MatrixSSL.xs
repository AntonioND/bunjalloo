#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include "matrixCommon.h"
#include "matrixSsl.h"

#include "const-c.inc"

/******************************************************************************/

/*
 * my_hv_store() helper macro to avoid writting hash key names twice or
 * hardcoding their length
#define myVAL_LEN(k)        (k), strlen((k))
 */

#define my_hv_store(myh1,myh2,myh3,myh4)	hv_store((myh1),(myh2),(strlen((myh2))),(myh3),(myh4))

/*
 * Hash which will contain perl's certValidate CODEREF and it second ARG
 * between matrixSslSetCertValidator() and matrixSslDeleteSession().
 * 
 * Hash format:
 *  key	    integer representation of $ssl (ssl_t *)
 *  value   [ \&cb_certValidate, $cb_certValidate_arg ]
 */
static HV *	certValidatorArg = NULL;

int appCertValidator(sslCertInfo_t *certInfo, void *arg)
{
	dSP;
	SV *	    callback;
	SV *	    callback_arg;
	AV *	    certs;
	int	    res;

	ENTER;
	SAVETMPS;
	
	/* Fetch perl's callback CODEREF and it arg from my (void *)arg. */
	if (arg==NULL || SvTYPE((SV *)arg) != SVt_PVAV || av_len((AV *)arg)!=1)
	    croak("appCertValidator: arg must be AV with 2 elements");
	callback	= *av_fetch((AV *)arg, 0, 0);
	callback_arg	= *av_fetch((AV *)arg, 1, 0);
	
	/* Convert (sslCertInfo_t *) structs into array of hashes. */
	certs = (AV *)sv_2mortal((SV *)newAV());
	for (; certInfo != NULL; certInfo=certInfo->next) {
	    HV *    sslCertInfo;
	    HV *    subjectAltName;
	    HV *    subject;
	    HV *    issuer;

	    subjectAltName = newHV();

/* MatrixSSL 1.8.6 removes these constants, replacing them with a new single "data" structure

	    if (certInfo->subjectAltName.dns != NULL)
		my_hv_store(subjectAltName, "dns",
		    newSVpv(certInfo->subjectAltName.dns, 0), 0);
	    if (certInfo->subjectAltName.uri != NULL)
		my_hv_store(subjectAltName, "uri",
		    newSVpv(certInfo->subjectAltName.uri, 0), 0);
	    if (certInfo->subjectAltName.email != NULL)
		my_hv_store(subjectAltName, "email",
		    newSVpv(certInfo->subjectAltName.email, 0), 0);
*/
	    
	    subject = newHV();
	    if (certInfo->subject.country != NULL)
		my_hv_store(subject, "country",
		    newSVpv(certInfo->subject.country, 0), 0);
	    if (certInfo->subject.state != NULL)
		my_hv_store(subject, "state",
		    newSVpv(certInfo->subject.state, 0), 0);
	    if (certInfo->subject.locality != NULL)
		my_hv_store(subject, "locality",
		    newSVpv(certInfo->subject.locality, 0), 0);
	    if (certInfo->subject.organization != NULL)
		my_hv_store(subject, "organization",
		    newSVpv(certInfo->subject.organization, 0), 0);
	    if (certInfo->subject.orgUnit != NULL)
		my_hv_store(subject, "orgUnit",
		    newSVpv(certInfo->subject.orgUnit, 0), 0);
	    if (certInfo->subject.commonName != NULL)
		my_hv_store(subject, "commonName",
		    newSVpv(certInfo->subject.commonName, 0), 0);

	    issuer = newHV();
	    if (certInfo->issuer.country != NULL)
		my_hv_store(issuer, "country",
		    newSVpv(certInfo->issuer.country, 0), 0);
	    if (certInfo->issuer.state != NULL)
		my_hv_store(issuer, "state",
		    newSVpv(certInfo->issuer.state, 0), 0);
	    if (certInfo->issuer.locality != NULL)
		my_hv_store(issuer, "locality",
		    newSVpv(certInfo->issuer.locality, 0), 0);
	    if (certInfo->issuer.organization != NULL)
		my_hv_store(issuer, "organization",
		    newSVpv(certInfo->issuer.organization, 0), 0);
	    if (certInfo->issuer.orgUnit != NULL)
		my_hv_store(issuer, "orgUnit",
		    newSVpv(certInfo->issuer.orgUnit, 0), 0);
	    if (certInfo->issuer.commonName != NULL)
		my_hv_store(issuer, "commonName",
		    newSVpv(certInfo->issuer.commonName, 0), 0);

	    sslCertInfo = newHV();
	    my_hv_store(sslCertInfo, "verified",
		newSViv(certInfo->verified), 0);
	    if (certInfo->notBefore != NULL)
		my_hv_store(sslCertInfo, "notBefore",
		    newSVpv(certInfo->notBefore, 0), 0);
	    if (certInfo->notAfter != NULL)
		my_hv_store(sslCertInfo, "notAfter",
		    newSVpv(certInfo->notAfter, 0), 0);
	    my_hv_store(sslCertInfo, "subjectAltName",
		newRV(sv_2mortal((SV *)subjectAltName)), 0);
	    my_hv_store(sslCertInfo, "subject",
		newRV(sv_2mortal((SV *)subject)), 0);
	    my_hv_store(sslCertInfo, "issuer",
		newRV(sv_2mortal((SV *)issuer)), 0);
	    /* 
	     * I can't imagine how serialNumber and sigHash can be used.
	     * They are in binary format and make things like
	     * Data::Dumper::Dumper($cert) looks ugly.
	     * 
	     * my_hv_store(sslCertInfo, "serialNumber", 
	     *	    newSVpvn(certInfo->serialNumber, certInfo->serialNumberLen), 0);
	     * my_hv_store(sslCertInfo, "sigHash", 
	     *	    newSVpvn(certInfo->sigHash, certInfo->sigHashLen), 0);
	     */
	    
	    av_push(certs, newRV(sv_2mortal((SV *)sslCertInfo)));
	}
	
	PUSHMARK(SP);
	XPUSHs(sv_2mortal(newRV((SV *)certs)));
	XPUSHs(callback_arg);
	PUTBACK;

	res = call_sv(callback, G_EVAL|G_SCALAR);
	
	SPAGAIN;
	
	if (res != 1)
	    croak("Internal error: perl callback doesn't return 1 scalar!");
	
	if (SvTRUE(ERRSV)) {
	    warn("%s", SvPV_nolen(ERRSV));
	    warn("die() in certValidate callback not allowed, continue...\n");
	    POPs;
	    res = -1;
	} else {
	    res = POPi;
	}

	PUTBACK;
	FREETMPS;
	LEAVE;

	return res;
}

MODULE = Crypt::MatrixSSL		PACKAGE = Crypt::MatrixSSL		

INCLUDE: const-xs.inc

PROTOTYPES: ENABLE


int
matrixSslOpen()


void
matrixSslClose()
    

int
matrixSslReadKeys(keys, certFile, privFile, privPass, trustedCAcertFiles)
	sslKeys_t *	    &keys    = SvOK(ST(0)) ? (sslKeys_t *)SvIV(ST(0)) : NULL;
       	char *		    certFile = SvOK(ST(1)) ? SvPV_nolen(ST(1)) : NULL;
	char *		    privFile = SvOK(ST(2)) ? SvPV_nolen(ST(2)) : NULL;
	char *		    privPass = SvOK(ST(3)) ? SvPV_nolen(ST(3)) : NULL;
	char *		    trustedCAcertFiles = SvOK(ST(4)) ? SvPV_nolen(ST(4)) : NULL;
    OUTPUT:
    	keys
        RETVAL


int
matrixSslReadKeysMem(keys, cert, priv, trustedCA)
	sslKeys_t *	    &keys = SvOK(ST(0)) ? (sslKeys_t *)SvIV(ST(0)) : NULL;
        SV *		    cert
	SV *		    priv
	SV *		    trustedCA
    INIT:
	unsigned char * certBuf;
	unsigned char * privBuf;
	unsigned char * trustedCABuf;
	STRLEN		certLen		= 0;
	STRLEN		privLen		= 0;
	STRLEN		trustedCALen	= 0;
    CODE:
	/* All bufs can contain \0, so SvPV must be used instead of strlen() */
	certBuf	     = SvOK(cert)      ? SvPV(cert, certLen)	       : NULL;
	privBuf	     = SvOK(priv)      ? SvPV(priv, privLen)	       : NULL;
	trustedCABuf = SvOK(trustedCA) ? SvPV(trustedCA, trustedCALen) : NULL;
        RETVAL = matrixSslReadKeysMem(&keys, certBuf, certLen, privBuf, privLen,
		    trustedCABuf, trustedCALen);
    OUTPUT:
    	keys
        RETVAL


void
matrixSslFreeKeys(keys)
	sslKeys_t *	    keys


int
matrixSslNewSession(ssl, keys, sessionId, flags)
	ssl_t *		    &ssl      = SvOK(ST(0)) ? (ssl_t *)SvIV(ST(0)) : NULL;
	sslKeys_t *	    keys
	sslSessionId_t *    sessionId = SvOK(ST(2)) ? (sslSessionId_t *)SvIV(ST(2)) : NULL;
	int		    flags
    OUTPUT:
    	ssl
        RETVAL


void
matrixSslDeleteSession(ssl)
	ssl_t *		    ssl
    INIT:
	SV *	    key;
    CODE:
	/* Free array with appCertValidator() args if it was registered. */
	key = sv_2mortal(newSViv((int)ssl));
	if (certValidatorArg==NULL) {
	    certValidatorArg = newHV();
	}
	if (hv_exists_ent(certValidatorArg, key, 0)) {
	    hv_delete_ent(certValidatorArg, key, G_DISCARD, 0);
	}
	
	matrixSslDeleteSession(ssl);


int
matrixSslDecode(ssl, inBuf, outBuf, error, alertLevel, alertDescription)
	ssl_t *		    ssl
	SV *		    inBuf
	SV *		    outBuf
	unsigned char	    &error	      = SvOK(ST(3)) ? SvUV(ST(3)) : 0;
	unsigned char	    &alertLevel	      = SvOK(ST(4)) ? SvUV(ST(4)) : 0;
	unsigned char	    &alertDescription = SvOK(ST(5)) ? SvUV(ST(5)) : 0;
    INIT:
	sslBuf_t    in;
	sslBuf_t    out;
	char	    buf[SSL_MAX_BUF_SIZE];
    CODE:
	in.buf	    = SvPV(inBuf, in.size);
	in.start    = in.buf;
	in.end	    = in.buf + in.size;

	out.size    = sizeof(buf);
	out.buf	    = buf;
	out.start   = out.buf;
	out.end	    = out.buf;
	
	RETVAL = matrixSslDecode(ssl, &in, &out, &error, &alertLevel, &alertDescription);

	/* append answer to the output */
	sv_catpvn_mg(outBuf, out.start, out.end-out.start);
	/* remove from the input whatever got processed */
	sv_setpvn_mg( inBuf,  in.start,  in.end- in.start);
    OUTPUT:
    	inBuf
	outBuf
    	error
	alertLevel
	alertDescription
        RETVAL


int
matrixSslHandshakeIsComplete(ssl)
	ssl_t *		    ssl


int
matrixSslEncode(ssl, inBuf, outBuf)
	ssl_t *		    ssl
	SV *		    inBuf
	SV *		    outBuf
    INIT:
    	STRLEN		inLen;
	unsigned char * inPtr;
	sslBuf_t	out;
	char		buf[SSL_MAX_BUF_SIZE];
    CODE:
	out.size    = sizeof(buf);
	out.buf	    = buf;
	out.start   = out.buf;
	out.end	    = out.buf;
	
	/* inBuf can contain \0, so SvPV must be used instead of strlen() */
	inPtr = SvPV(inBuf, inLen);
	
	RETVAL = matrixSslEncode(ssl, inPtr, inLen, &out);

	/* append answer to the output */
	sv_catpvn_mg(outBuf, out.start, out.end-out.start);
    OUTPUT:
    	outBuf
	RETVAL


int
matrixSslEncodeClosureAlert(ssl, outBuf)
	ssl_t *		    ssl
	SV *		    outBuf
    INIT:
	sslBuf_t	out;
	char		buf[SSL_MAX_BUF_SIZE];
    CODE:
	out.size    = sizeof(buf);
	out.buf	    = buf;
	out.start   = out.buf;
	out.end	    = out.buf;

	RETVAL = matrixSslEncodeClosureAlert(ssl, &out);

	/* append answer to the output */
	sv_catpvn_mg(outBuf, out.start, out.end-out.start);
    OUTPUT:
    	outBuf
	RETVAL


int
matrixSslEncodeClientHello(ssl, outBuf, cipherSuite)
	ssl_t *             ssl
	SV *		    outBuf
	unsigned short	    cipherSuite
    INIT:
	sslBuf_t	out;
	char		buf[SSL_MAX_BUF_SIZE];
    CODE:
	out.size    = sizeof(buf);
	out.buf	    = buf;
	out.start   = out.buf;
	out.end	    = out.buf;

        RETVAL = matrixSslEncodeClientHello(ssl, &out, cipherSuite);

	/* append answer to the output */
	sv_catpvn_mg(outBuf, out.start, out.end-out.start);
    OUTPUT:
    	outBuf
        RETVAL


int
matrixSslEncodeHelloRequest(ssl, outBuf)
	ssl_t *             ssl
	SV *		    outBuf
    INIT:
	sslBuf_t	out;
	char		buf[SSL_MAX_BUF_SIZE];
    CODE:
	out.size    = sizeof(buf);
	out.buf	    = buf;
	out.start   = out.buf;
	out.end	    = out.buf;
	
        RETVAL = matrixSslEncodeHelloRequest(ssl, &out);

	/* append answer to the output */
	sv_catpvn_mg(outBuf, out.start, out.end-out.start);
    OUTPUT:
    	outBuf
        RETVAL


void
matrixSslSetSessionOption(ssl, option, arg)
	ssl_t *		    ssl
       	int		    option
	void *		    arg = SvOK(ST(2)) ? SvPV_nolen(ST(2)) : NULL;


int
matrixSslGetSessionId(ssl, sessionId)
	ssl_t *		    ssl
	sslSessionId_t *    &sessionId = SvOK(ST(1)) ? (sslSessionId_t *)SvIV(ST(1)) : NULL;
    OUTPUT:
    	sessionId
    	RETVAL


void
matrixSslFreeSessionId(sessionId)
	sslSessionId_t *    sessionId

	
void 
matrixSslSetCertValidator(ssl, callback, callback_arg)
	ssl_t *		    ssl
	SV *		    callback
	SV *		    callback_arg
    INIT:
	AV *		arg;
	SV *		key;
    CODE:
	/*
	 * Create array with both callback and callback_arg, so they both
	 * can be passed into matrixSslSetCertValidator() as single (void *)
	 */
	arg = newAV();
	av_push(arg, SvREFCNT_inc(callback));
	av_push(arg, SvREFCNT_inc(callback_arg));
	/*
	 * Save this array into global hash certValidatorArg to take
	 * ownership of this array and be able to free it later.
	 */
	key = sv_2mortal(newSViv((int)ssl));
	if (certValidatorArg==NULL) {
	    certValidatorArg = newHV();
	}
	if (hv_exists_ent(certValidatorArg, key, 0)) {
	    hv_delete_ent(certValidatorArg, key, G_DISCARD, 0);
	}
	hv_store_ent(certValidatorArg, key, (SV *)arg, 0);
	/* Register callback with this array as arg. */
	matrixSslSetCertValidator(ssl, appCertValidator, (void *)arg);


void
matrixSslGetAnonStatus(ssl, anonArg)
	ssl_t *		    ssl
	int 		    &anonArg
    OUTPUT:
	anonArg


void
matrixSslAssignNewKeys(ssl, keys)
	ssl_t *		    ssl
	sslKeys_t *	    keys


int
matrixSslSetResumptionFlag(ssl, flag)
	ssl_t *		    ssl
	char		    flag


int
matrixSslGetResumptionFlag(ssl, flag)
	ssl_t *		    ssl
	char		    &flag
    OUTPUT:
	flag
	RETVAL


