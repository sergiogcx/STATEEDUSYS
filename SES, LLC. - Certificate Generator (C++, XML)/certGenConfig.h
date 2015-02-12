/* 
 * File:   certGenConfig.h
 * Author: sergio
 *
 * Created on June 10, 2012, 10:32 AM
 */

#ifndef CERTGENCONFIG_H
#define	CERTGENCONFIG_H

#include <string>

using namespace std;

class certGenConfig {
public:
    certGenConfig();
    certGenConfig(const certGenConfig& orig);
    virtual ~certGenConfig();
    
    void getBackgroundImageName();
    void getSignatureImageName();
    
    // GetElement
    string getElementString(string tag);
    int getElementInt(string tag);
        
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
    
    string _fontname;
    string _backgroundImageFilename;
    string _signatureImageFilename;
    
    string _urlAddress;
    string _companyName;
    string _phoneNumber;
    string _managerCode;
    string _licenseNumber;
    
    int _urlAddress_fontsize;
    int _companyName_fontsize;
    int _phoneNumber_fontsize;
    int _managerCode_fontsize;
    int _licenseNumber_fontsize;
    
    int certificateWidth;
    int certificateHeight;
    int certificateMarginsA;
    int certificateMarginsB;
    int certificateMarginsY;
    
    int textCorePosX;
    int textCorePosY;
    
    int textNamePosX;
    int textNamePosY;
    int textNameSize;
    
    int textManagerNamePosX;
    int textManagerNamePosY;
    
    int textCompanyNamePosX;
    int textCompanyNamePosY;
    
    int textUrlAddressPosX;
    int textUrlAddressPosY;
    
    int textPhoneNumberPosX;
    int textPhoneNumberPosY;
    
    int textLicenseNumberPosX;
    int textLicenseNumberPosY;
        
    int textIssuedDatePosX;
    int textIssuedDatePosY;
    int textIssuedDateSize;
    
    int textExpirationDatePosX;
    int textExpirationDatePosY;
    int textExpirationDateSize;
    
    int imageSignaturePosX;
    int imageSignaturePosY;    
};

#endif	/* CERTGENCONFIG_H */

