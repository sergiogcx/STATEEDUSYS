/* 
 * File:   envelopeConfig.h
 * Author: sergio
 *
 * Created on June 18, 2012, 4:06 PM
 */

#ifndef ENVELOPECONFIG_H
#define	ENVELOPECONFIG_H

#include <string>

using namespace std;

struct foobar_color {
    float r, g, b;
};

struct foobar {
    int x, y, w, h;
};

class envelopeConfig {
public:
    envelopeConfig();
    envelopeConfig(const envelopeConfig& orig);
    virtual ~envelopeConfig();

    void loadXmlFile(string filename);
    
    void setDebugMode(bool newMode) {
        this->debugMode = newMode;
    }
    
    bool getDebugMode() {
        return this->debugMode;
    }
    
public:

    int loaded;
    bool debugMode;
    
    string _font;
    string _logo;
    string _mail;
    
    string _title,
           _company,
           _company_address,
           _company_address_csz,
           _company_phone;
    
    int _pageWidth,
        _pageHeight;
    
    int _logo_pos_x,
        _logo_pos_y,
        _logo_pos_h,
        _logo_pos_w;
    
    int _mail_pos_x,
        _mail_pos_y,
        _mail_pos_h,
        _mail_pos_w;
    
    int _title_x,
        _title_y,
        _title_size;
    
    int _company_x,
        _company_y,
        _company_size;
    
    int _company_address_x,
        _company_address_y,
        _company_address_size;
    
    int _company_address_csz_x,
        _company_address_csz_y,
        _company_address_csz_size;
    
    int _company_phone_x,
        _company_phone_y,
        _company_phone_size;
    
    int _users_name_x,
        _users_name_y,
        _users_name_size;
    
    int _users_address_x,
        _users_address_y,
        _users_address_size;
    
    int _users_address_csz_x,
        _users_address_csz_y,
        _users_address_csz_size;
    
    foobar_color _foobar_color;
    
    foobar _foobar_a,
           _foobar_b;
};

#endif	/* ENVELOPECONFIG_H */

