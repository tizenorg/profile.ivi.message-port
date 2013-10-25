/* vi: set et sw=4 ts=4 cino=t0,(0: */
/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2013 Intel Corporation.
 *
 * Contact: Amarnath Valluri <amarnath.valluri@linux.intel.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <message-port.h>
#include <bundle.h>
#include <string>
#include <iostream>

using namespace std;

class LocalPort {
protected:
    static void OnMessage (int id, const char *r_app, const char *r_port, bool r_is_trusted, bundle *data)
    {
        cout << "Message received" << endl;
    }

public:
    LocalPort (const std::string &name, bool is_trusted)
        : m_name (name), m_trusted(is_trusted) { }

    bool Register () {
        int res ;
        res = m_trusted ? messageport_register_trusted_local_port (m_name.c_str(), OnMessage)
                        : messageport_register_local_port (m_name.c_str(), OnMessage);

        if (res < 0) {
            cerr << "Failed to register port '"<< m_name << "'";
            return false;
        }
        
        return true;
    }

    bool SendMessage (const std::string &app_id, const std::string &port_name, bool is_trusted, bundle *data)
    {
        messageport_error_e res ;
        
        res = is_trusted ? messageport_send_bidirectional_trusted_message (m_port_id, app_id.c_str(), port_name.c_str(), data)
                         : messageport_send_bidirectional_message (m_port_id, app_id.c_str(), port_name.c_str(), data);
        if (res < 0) {
            cerr << "Fail to send bidirectional message to '" << app_id << "/" << port_name << ":" << res ;
            return false;
        }

        return true;
    }

private:
    std::string m_name;
    bool        m_trusted;
    int         m_port_id;
};

int main (int argc, const char *argv[])
{
    GMainLoop *m_loop = g_main_loop_new (NULL, FALSE);

    LocalPort port1 ("test_port1", false);

    port1.Register ();

    g_main_loop_run (m_loop);

    g_main_loop_unref (m_loop);

    return 0;
}

