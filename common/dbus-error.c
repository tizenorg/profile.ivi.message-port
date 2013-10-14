#include "dbus-error.h"

#include <gio/gio.h>

#define MSGPORT_ERROR_DOMAIN "messagport"
#define _PREFIX              "org.tizen.MessagePort.Error"

GDBusErrorEntry __msgport_errors [] = {
    {MSGPORT_ERROR_IO_ERROR,             _PREFIX".IOError"},
    {MSGPORT_ERROR_INVALID_PARAMS,       _PREFIX".InvalidParams"},
    {MSGPORT_ERROR_OUT_OF_MEMORY,        _PREFIX".OutOfMemory"},
    {MSGPORT_ERROR_NOT_FOUND,            _PREFIX".NotFound"},
    {MSGPORT_ERROR_ALREADY_EXISTING,     _PREFIX".AlreadyExisting"},
    {MSGPORT_ERROR_CERTIFICATE_MISMATCH, _PREFIX".CertificateMismatch"},
    {MSGPORT_ERROR_UNKNOWN,              _PREFIX ".Unknown"}
};

GQuark
msgport_error_quark (void)
{
    static volatile gsize quark_volatile = 0;

    if (!quark_volatile) {
        g_dbus_error_register_error_domain (MSGPORT_ERROR_DOMAIN, 
                                            &quark_volatile,
                                            __msgport_errors,
                                            G_N_ELEMENTS (__msgport_errors));
    }

    return quark_volatile;
}
