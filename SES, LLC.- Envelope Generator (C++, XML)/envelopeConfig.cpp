/* 
 * File:   envelopeConfig.cpp
 * Author: sergio
 * 
 * Created on June 18, 2012, 4:06 PM
 */
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>

#include "tinyxml2.h"
#include "envelopeConfig.h"

using namespace tinyxml2;

envelopeConfig::envelopeConfig() {
}

envelopeConfig::envelopeConfig(const envelopeConfig& orig) {
}

envelopeConfig::~envelopeConfig() {
}

void envelopeConfig::loadXmlFile(string filename) {
    XMLDocument doc;
    string backgroundImage, signatureImage;
    int positiveNo, negativeNo;
    
    loaded = doc.LoadFile( filename.c_str() );
    
    // 1) Font
    if(this->getDebugMode()) cout << "Reading font: ";
        _font = doc.FirstChildElement( "envelope" )->FirstChildElement( "font" )->GetText();
    if(this->getDebugMode()) cout << ".. got: " << _font << endl;
    
    // 2) Logo
    if(this->getDebugMode()) cout << "Reading logo: ";
        _logo = doc.FirstChildElement( "envelope" )->FirstChildElement( "logo" )->GetText();
        _logo_pos_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "logo" )->IntAttribute("x");
        _logo_pos_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "logo" )->IntAttribute("y");
        _logo_pos_w = doc.FirstChildElement( "envelope" )->FirstChildElement( "logo" )->IntAttribute("w");
        _logo_pos_h = doc.FirstChildElement( "envelope" )->FirstChildElement( "logo" )->IntAttribute("h");
    if(this->getDebugMode()) cout << ".. got: " << _logo << " @ (" << _logo_pos_x << ","  << _logo_pos_y << ","<< _logo_pos_w << ","<< _logo_pos_h << ")" << endl;
    
    // 3) Mail Icon
    if(this->getDebugMode()) cout << "Reading Mail Icon: ";
        _mail = doc.FirstChildElement( "envelope" )->FirstChildElement( "mail_icon" )->GetText();
        _mail_pos_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "mail_icon" )->IntAttribute("x");
        _mail_pos_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "mail_icon" )->IntAttribute("y");
        _mail_pos_w = doc.FirstChildElement( "envelope" )->FirstChildElement( "mail_icon" )->IntAttribute("w");
        _mail_pos_h = doc.FirstChildElement( "envelope" )->FirstChildElement( "mail_icon" )->IntAttribute("h");
    if(this->getDebugMode()) cout << ".. got: " << _mail << " @ (" << _mail_pos_x << ","  << _mail_pos_y << ","<< _mail_pos_w << ","<< _mail_pos_h << ")" << endl;
        
    // 4) Page Dimensions
    if(this->getDebugMode()) cout << "Reading Page Dimensions: ";
        _pageWidth = atoi(doc.FirstChildElement( "envelope" )->FirstChildElement( "width" )->GetText());
        _pageHeight = atoi(doc.FirstChildElement( "envelope" )->FirstChildElement( "height" )->GetText());
    if(this->getDebugMode()) cout << ".. got: " << "w: " << _pageWidth << ", h: " << _pageHeight << endl;
   
    
    // 5) Foobar Color
    if(this->getDebugMode()) cout << "Reading Foobar Color: ";
        this->_foobar_color.r = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_color" )->FloatAttribute("r");
        this->_foobar_color.g = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_color" )->FloatAttribute("g");
        this->_foobar_color.b = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_color" )->FloatAttribute("b");
    if(this->getDebugMode()) cout << ".. got: " << " rgb (" << _foobar_color.r << "," << _foobar_color.g << "," << _foobar_color.b << ")" << endl;
        
    // 6) Foobar A
    if(this->getDebugMode()) cout << "Reading Foobar A: ";
        this->_foobar_a.x = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_a" )->IntAttribute("x");
        this->_foobar_a.y = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_a" )->IntAttribute("y");
        this->_foobar_a.w = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_a" )->IntAttribute("w");
        this->_foobar_a.h = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_a" )->IntAttribute("h");
    if(this->getDebugMode()) cout << ".. got: " << " xywh (" << _foobar_a.x << "," << _foobar_a.y << "," << _foobar_a.w << "," << _foobar_a.h << ")" << endl;
    
    // 6) Foobar B
    if(this->getDebugMode()) cout << "Reading Foobar B: ";
        this->_foobar_b.x = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_b" )->IntAttribute("x");
        this->_foobar_b.y = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_b" )->IntAttribute("y");
        this->_foobar_b.w = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_b" )->IntAttribute("w");
        this->_foobar_b.h = doc.FirstChildElement( "envelope" )->FirstChildElement( "foobar_b" )->IntAttribute("h");
    if(this->getDebugMode()) cout << ".. got: " << " xywh (" << _foobar_b.x << "," << _foobar_b.y << "," << _foobar_b.w << "," << _foobar_b.h << ")" << endl;
                
    // 8) Title
    if(this->getDebugMode()) cout << "Reading Page Title: ";
        _title = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title" )->GetText();
        _title_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title" )->IntAttribute("x");
        _title_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title" )->IntAttribute("y");
        _title_size = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title" )->IntAttribute("size");
    if(this->getDebugMode()) cout << ".. got: " << _title << " @ (" << _title_x << "," << _title_y << "," << _title_size << ")" << endl;
    
        
    // 9) Company
    if(this->getDebugMode()) cout << "Reading Company: ";
        _company = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company" )->GetText();
        _company_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company" )->IntAttribute("x");
        _company_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company" )->IntAttribute("y");
        _company_size = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company" )->IntAttribute("size");
    if(this->getDebugMode()) cout << ".. got: " << _company << " @ (" << _company_x << "," << _company_y << "," << _company_size << ")" << endl;
        
    // 10) Company Address
    if(this->getDebugMode()) cout << "Reading Company Address: ";
        _company_address = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company_address" )->GetText();
        _company_address_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company_address" )->IntAttribute("x");
        _company_address_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company_address" )->IntAttribute("y");
        _company_address_size = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company_address" )->IntAttribute("size");
    if(this->getDebugMode()) cout << ".. got: " << _company_address << " @ (" << _company_address_x << "," << _company_address_y << "," << _company_address_size << ")" << endl;
    
    // 11) City, State & ZIP
    if(this->getDebugMode()) cout << "Reading City, State & ZIP: ";
        _company_address_csz = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company_csz" )->GetText();
        _company_address_csz_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company_csz" )->IntAttribute("x");
        _company_address_csz_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company_csz" )->IntAttribute("y");
        _company_address_csz_size = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_company_csz" )->IntAttribute("size");
    if(this->getDebugMode()) cout << ".. got: " << _company_address_csz << " @ (" << _company_address_csz_x << "," << _company_address_csz_y << "," << _company_address_csz_size << ")" << endl;
        
        
    // 12) Company Phone
    if(this->getDebugMode()) cout << "Reading Company Phone: ";
        _company_phone = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_phone" )->GetText();
        _company_phone_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_phone" )->IntAttribute("x");
        _company_phone_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_phone" )->IntAttribute("y");
        _company_phone_size = doc.FirstChildElement( "envelope" )->FirstChildElement( "ses_title_phone" )->IntAttribute("size");
    if(this->getDebugMode()) cout << ".. got: " << _company_phone << " @ (" << _company_phone_x << "," << _company_phone_y << "," << _company_phone_size << ")" << endl;    
        
        
    // 13) User's Name
    if(this->getDebugMode()) cout << "Reading User's Name: ";
        _users_name_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "usr_name" )->IntAttribute("x");
        _users_name_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "usr_name" )->IntAttribute("y");
        _users_name_size = doc.FirstChildElement( "envelope" )->FirstChildElement( "usr_name" )->IntAttribute("size");
    if(this->getDebugMode()) cout << ".. got: " << " User's Name @ (" << _company_phone_x << "," << _company_phone_y << "," << _company_phone_size << ")" << endl;
    
        
    // 14) User's Address
    if(this->getDebugMode()) cout << "Reading User's Address: ";
        _users_address_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "usr_address" )->IntAttribute("x");
        _users_address_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "usr_address" )->IntAttribute("y");
        _users_address_size = doc.FirstChildElement( "envelope" )->FirstChildElement( "usr_address" )->IntAttribute("size");
    if(this->getDebugMode()) cout << ".. got: " << "User's Address" << " @ (" << _users_address_x << "," << _users_address_y << "," << _users_address_size << ")" << endl;
        
        
    // 15) User's CSZ
    if(this->getDebugMode()) cout << "Reading User's CSZ: ";
        _users_address_csz_x = doc.FirstChildElement( "envelope" )->FirstChildElement( "usr_address_csz" )->IntAttribute("x");
        _users_address_csz_y = doc.FirstChildElement( "envelope" )->FirstChildElement( "usr_address_csz" )->IntAttribute("y");
        _users_address_csz_size = doc.FirstChildElement( "envelope" )->FirstChildElement( "usr_address_csz" )->IntAttribute("size");
    if(this->getDebugMode()) cout << ".. got: " << "User's CSZ" << " @ (" << _users_address_csz_x << "," << _users_address_csz_y << "," << _users_address_csz_size << ")" << endl;
}

