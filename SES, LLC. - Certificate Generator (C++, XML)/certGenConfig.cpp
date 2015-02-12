/* 
 * File:   certGenConfig.cpp
 * Author: sergio
 * 
 * Created on June 10, 2012, 10:32 AM
 */
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>

#include "tinyxml2.h"
#include "certGenConfig.h"

using namespace tinyxml2;

certGenConfig::certGenConfig() {
    this->setDebugMode(false);
}

certGenConfig::certGenConfig(const certGenConfig& orig) {
}

certGenConfig::~certGenConfig() {
}

void certGenConfig::loadXmlFile(string filename) {
    
    XMLDocument doc;
    string backgroundImage, signatureImage;
    int positiveNo, negativeNo;
    
    loaded = doc.LoadFile( filename.c_str() );

    if(this->getDebugMode()) cout << "Reading XML ...";
    
    
    if(this->getDebugMode()) cout << "Reading certificate_managercode: ";
        _managerCode = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_managercode" )->GetText();
        _managerCode_fontsize = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_managercode" )->IntAttribute("fontsize");
    if(this->getDebugMode()) cout << ".. got: " << _managerCode << "/" << _managerCode_fontsize << endl;
    
    if(this->getDebugMode()) cout << "Reading certificate_website_url: ";
        _urlAddress = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_website_url" )->GetText();
        _urlAddress_fontsize = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_website_url" )->IntAttribute("fontsize");
    if(this->getDebugMode()) cout << ".. got: " << _urlAddress << "/" << _urlAddress_fontsize << endl;
    
    if(this->getDebugMode()) cout << "Reading certificate_companyname: ";
        _companyName = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_companyname" )->GetText();
        _companyName_fontsize = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_companyname" )->IntAttribute("fontsize");
    if(this->getDebugMode()) cout << ".. got: " << _companyName << "/" << _companyName_fontsize << endl;
    
    if(this->getDebugMode()) cout << "Reading certificate_companynumber: ";
        _phoneNumber = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_companynumber" )->GetText();
        _phoneNumber_fontsize = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_companynumber" )->IntAttribute("fontsize");
    if(this->getDebugMode()) cout << ".. got: " << _phoneNumber << "/" << _phoneNumber_fontsize << endl;
    
    if(this->getDebugMode()) cout << "Reading certificate_licensenumber: ";
        _licenseNumber = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_licensenumber" )->GetText();
        _licenseNumber_fontsize = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_licensenumber" )->IntAttribute("fontsize");
    if(this->getDebugMode()) cout << ".. got: " << _licenseNumber << "/" << _licenseNumber_fontsize << endl;
    
    
    
    if(this->getDebugMode()) cout << "Reading certificate_background: ";
        _backgroundImageFilename = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_background" )->GetText();
    if(this->getDebugMode()) cout << ".. got: " << _backgroundImageFilename << endl;
    
    if(this->getDebugMode()) cout << "Reading certificate_signature: ";
        _signatureImageFilename = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_signature" )->GetText();
    if(this->getDebugMode()) cout << ".. got: " << _signatureImageFilename << endl;
    
    if(this->getDebugMode()) cout << "Reading certificate_font: ";
        _fontname = doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_font" )->GetText();
    if(this->getDebugMode()) cout << ".. got: " << _fontname << endl;
    
    
    
    
    
    
    if(this->getDebugMode()) cout << "Reading certificate_width[height]: ";
        certificateWidth  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_width" )->GetText());
        certificateHeight = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_height" )->GetText());
    if(this->getDebugMode()) cout << ".. got: W:" << certificateWidth << " H:" << certificateHeight << endl;
    
    if(this->getDebugMode()) cout << "Reading certificate_margins[a,b,y]: ";
        certificateMarginsA  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_margins_a" )->GetText());
        certificateMarginsB  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_margins_b" )->GetText());
        certificateMarginsY  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "certificate_margins_y" )->GetText());
    if(this->getDebugMode()) cout << ".. got: a:" << certificateMarginsA << ", b: " << certificateMarginsB << ", y: " << certificateMarginsY << endl;
        
    
    if(this->getDebugMode()) cout << "Reading text_core_pos_x[y]: ";
        textCorePosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_core_pos_x" )->GetText());
        textCorePosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_core_pos_y" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << textCorePosX << " Y:" << textCorePosY << endl;
    
    
    if(this->getDebugMode()) cout << "Reading text_name_pos_x[y][size]: ";
        textNamePosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_name_pos_x" )->GetText());
        textNamePosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_name_pos_y" )->GetText());
        textNameSize = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_name_size" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << textNamePosX << " Y:" << textNamePosY << " S:" << textNameSize <<  endl;
    
    if(this->getDebugMode()) cout << "Reading text_managername_pos_x[y]: ";
        textManagerNamePosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_managername_pos_x" )->GetText());
        textManagerNamePosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_managername_pos_y" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << textManagerNamePosX << " Y:" << textManagerNamePosY  << endl;
    
    if(this->getDebugMode()) cout << "Reading text_companyname_pos_x[y]: ";
        textCompanyNamePosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_companyname_pos_x" )->GetText());
        textCompanyNamePosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_companyname_pos_y" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << textCompanyNamePosX << " Y:" << textCompanyNamePosY  << endl;

    if(this->getDebugMode()) cout << "Reading text_urladdress_pos_x[y]: ";
        textUrlAddressPosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_urladdress_pos_x" )->GetText());
        textUrlAddressPosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_urladdress_pos_y" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << textUrlAddressPosX << " Y:" << textUrlAddressPosY  << endl;

    if(this->getDebugMode()) cout << "Reading text_phonenumber_pos_x[y]: ";
        textPhoneNumberPosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_phonenumber_pos_x" )->GetText());
        textPhoneNumberPosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_phonenumber_pos_y" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << textPhoneNumberPosX << " Y:" << textPhoneNumberPosY  << endl;
    
    if(this->getDebugMode()) cout << "Reading text_licensenumber_pos_x[y]: ";
        textLicenseNumberPosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_licensenumber_pos_x" )->GetText());
        textLicenseNumberPosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_licensenumber_pos_y" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << textLicenseNumberPosX << " Y:" << textLicenseNumberPosY  << endl;
    
    
    
    
    if(this->getDebugMode()) cout << "Reading text_issueddate_pos_x[y][size]: ";
        textIssuedDatePosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_issueddate_pos_x" )->GetText());
        textIssuedDatePosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_issueddate_pos_y" )->GetText());
        textIssuedDateSize = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_issueddate_size" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << textIssuedDatePosX << " Y:" << textIssuedDatePosY << "/" << textIssuedDateSize << endl;
    
    if(this->getDebugMode()) cout << "Reading text_expirationdate_pos_x[y][size]: ";
        textExpirationDatePosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_expirationdate_pos_x" )->GetText());
        textExpirationDatePosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_expirationdate_pos_y" )->GetText());
        textExpirationDateSize = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "text_expirationdate_size" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << textExpirationDatePosX << " Y:" << textExpirationDatePosY << "/" << textExpirationDateSize << endl;

    
    
    if(this->getDebugMode()) cout << "Reading image_signature_pos_x[y]: ";
        imageSignaturePosX  = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "image_signature_pos_x" )->GetText());
        imageSignaturePosY = atoi(doc.FirstChildElement( "configuration" )->FirstChildElement( "image_signature_pos_y" )->GetText());
    if(this->getDebugMode()) cout << ".. got: X:" << imageSignaturePosX << " Y:" << imageSignaturePosY  << endl;
    
    
    if(this->getDebugMode()) cout << "Done loading config file '" << filename << "'!"<< endl;
}
