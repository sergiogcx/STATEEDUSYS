/*
 * << Certificate Generator >> -- certGen.cpp
 *
 * Copyright (c) 2012 Sergio Garcia <sergio.garcia@statecertify.com>
 *
 */

#include <setjmp.h>
#include <hpdf.h>

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <cmath>

#include "certGenConfig.h"

using namespace std;
static bool debug_mode = false;
static bool debug_render = false;
static string config_file = "";

#ifndef HPDF_NOPNGLIB

jmp_buf env;

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler  (HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void         *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

void print_image (HPDF_Page page,  float x, float y, HPDF_Image image, int w, int h)
{
    char buf[255];

    if(debug_render) {
        HPDF_Page_MoveTo (page, x, y - 10);
        HPDF_Page_LineTo (page, x, y + 10);
        HPDF_Page_MoveTo (page, x - 10, y);
        HPDF_Page_LineTo (page, x + 10, y);
        HPDF_Page_Stroke (page);

        HPDF_Page_SetFontAndSize (page, HPDF_Page_GetCurrentFont (page), 8);
        HPDF_Page_SetRGBFill (page, 0, 0, 0);

        HPDF_Page_BeginText (page);

#ifdef __WIN32__
        _snprintf(buf, 255, "(x=%d,y=%d)", (int)x, (int)y);
#else
        snprintf(buf, 255, "(x=%d,y=%d)", (int)x, (int)y);
#endif /* __WIN32__ */
        HPDF_Page_MoveTextPos (page, x - HPDF_Page_TextWidth (page, buf) - 5,
                y - 10);
        HPDF_Page_ShowText (page, buf);
        HPDF_Page_EndText (page);
    }
	
    int iw = 0;
    int ih = 0;

    if (w == 0) iw = HPDF_Image_GetWidth (image);
    else iw = w;

    if (h == 0) ih = HPDF_Image_GetHeight (image);
    else ih = h;	
	
    /* Draw image to the canvas. (normal-mode with actual size.)*/
    HPDF_Page_DrawImage (page, image, x, y, iw, ih);
	
    return;
}

void print_cross(HPDF_Page page, float x, float y)
{
    HPDF_Page_SetRGBFill (page, 24, 0, 0);
    HPDF_Page_MoveTo (page, x, y - 10);
    HPDF_Page_LineTo (page, x, y + 10);
    HPDF_Page_MoveTo (page, x - 10, y);
    HPDF_Page_LineTo (page, x + 10, y);
    HPDF_Page_Stroke (page);
}

void print_description (HPDF_Page page, float x, float y, const char *text, int size)
{
    char buf[255];
    HPDF_Page_SetFontAndSize (page, HPDF_Page_GetCurrentFont (page), size);
    
    if(debug_render) {
        HPDF_Page_SetRGBFill (page, 0, 0, 0);
        HPDF_Page_MoveTo (page, x, y - 10);
        HPDF_Page_LineTo (page, x, y + 10);
        HPDF_Page_MoveTo (page, x - 10, y);
        HPDF_Page_LineTo (page, x + 10, y);
        HPDF_Page_Stroke (page);

        HPDF_Page_BeginText (page);

#ifdef __WIN32__
        _snprintf(buf, 255, "(x=%d,y=%d)", (int)x, (int)y);
#else
        snprintf(buf, 255, "(x=%d,y=%d)", (int)x, (int)y);
#endif /* __WIN32__ */
        HPDF_Page_MoveTextPos (page, x - HPDF_Page_TextWidth (page, buf) - 5, y - 10);
        HPDF_Page_ShowText (page, buf);
        HPDF_Page_EndText (page);
    }
    HPDF_Page_BeginText (page);
    HPDF_Page_MoveTextPos (page, x, y);
    HPDF_Page_ShowText (page, text);
    HPDF_Page_EndText (page);
}

bool doesFileExist(string filename) {
    cout << "Checking if " << filename << " exists...";
    ifstream my_file(filename.c_str());
    cout << (my_file.good() ? "Yep" : "Nope") << endl;
    return my_file.good();
}

int main (int argc, char **argv)
{
    // Main Config Object
    certGenConfig mainConfig;
    
    // Arguments & filenames
    vector<string> arguments;
    string outputPdfFile;
    string intputFileName;
    int usersSetLimit = 0;
    
    // PDF Document
    HPDF_Doc  pdf;
    HPDF_Font font;
    HPDF_Page page;
    HPDF_Destination dst;
    
    
    // Loading arguments 
    cout << "Initializing:" << endl;
    for(int n = 0; n < argc; n++) {
        cout << n << ") " << argv[n] <<  " (Diff:"<<((argc-1)-n)<<")" << endl;
        arguments.push_back(argv[n]);
        
        if(arguments[n]=="--verbose") debug_mode=true;
        if(arguments[n]=="--renderdebug") debug_render=true;
        
        if(arguments[n]=="--config") {
            
            // Check if there is a filename specified
            if ((argc-1)-n > 0) {
                // Check if the filename exists
                
                if(doesFileExist(argv[n+1])==true) {
                    config_file = argv[n+1];
                    if(debug_mode) cout << "Config File: " << config_file << endl;
                } else {
                    cerr << "Config file '" << argv[n+1] << "' does not exist as specified! Shutting down!..." << endl;
                    return 0;
                }
                
            } else {
                cerr << "No config file specified! Shutting down!..." << endl;
                return 0;
            }
        }
        
        if(arguments[n]=="--input") {
            // Check if there is a filename specified
            if ((argc-1)-n > 0) {
                // Check if the filename exists
                if(doesFileExist(argv[n+1])==true) {
                    intputFileName = argv[n+1];
                    if(debug_mode) cout << "Flag '--input' points: " << intputFileName << endl;
                } else {
                    cerr << "Flag '--input' points to '" << argv[n+1] << "' & does not exist as specified! Shutting down!..." << endl;
                    return 0;
                }
                
            } else {
                cerr << "Missing --input flag! Shutting down!..." << endl;
                return 0;
            }
        }
        
        if(arguments[n]=="--output") {
            // Check if there is a filename specified
            if ((argc-1)-n > 0) {
                // Check if the filename exists
                outputPdfFile = argv[n+1]; 
                // cerr << "--output flag identified, has been set to: " << outputPdfFile;
            } else {
                cerr  << "No output file specified,... rendering in local directory!" << endl;
            }
        }
        
        if(arguments[n]=="--limit") {
            // Check if there is a filename specified
            if ((argc-1)-n > 0) {
                // Check if the filename exists
                usersSetLimit = atoi(argv[n+1]); 
            } else {
                cerr  << "No limit specified,... ignoring flag!" << endl;
            }
        }
        
    }

    if(config_file=="") {
            cerr << "--config not specified! Shutting down!..." << endl;
            return 0;
    }
    
    
    // Setting up main config operation ... 
    mainConfig.setDebugMode(debug_mode);
    mainConfig.loadXmlFile(config_file);
    
    
    
    if(outputPdfFile.length() == 0) {
        outputPdfFile = arguments[0] + ".pdf"; // Only if empty
        cerr << "Output filename field seems to be empty, has been set to: " << outputPdfFile;
    }
    
    if(intputFileName.length()==0) {
        cerr << "Missing --input flag, shutting down!" << endl;
        return 0;
    }
    
    if(debug_mode) cout << "Output File Name: " << outputPdfFile << endl;
    if(debug_mode) cout << "Input File Name: " << intputFileName << endl;

    if(debug_mode) cout << "Creating PDF instance" << endl;
    pdf = HPDF_New (error_handler, NULL);
    if (!pdf) {
        if(debug_mode) cout << "Error: cannot create PdfDoc object!" << endl;
        return 1;
    }

    /* error-handler */
    if (setjmp(env)) {
        HPDF_Free (pdf);
        return 1;
    }

    if(debug_mode) cout << "Setting up compression mode." << endl;
    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    if(debug_mode) cout << "Setting up fonts!" << endl;
    /* create default-font */
    font = HPDF_GetFont (pdf, mainConfig._fontname.c_str(), NULL);
    
    
    
    /*
     * Variables Needed for rendering in right positions
     * 
     */
    
    if(debug_mode) cout << "Loading background & signature images...!" << endl;
    
    HPDF_Image background;
    HPDF_Image signature;

    /* load image file. */
    #ifndef __WIN32__
    background = HPDF_LoadPngImageFromFile (pdf, mainConfig._backgroundImageFilename.c_str());
    #else
    background = HPDF_LoadPngImageFromFile (pdf, mainConfig._backgroundImageFilename.c_str());
    #endif

    /* load image file. */
    #ifndef __WIN32__
    signature = HPDF_LoadPngImageFromFile (pdf, mainConfig._signatureImageFilename.c_str());
    #else
    signature = HPDF_LoadPngImageFromFile (pdf, mainConfig._signatureImageFilename.c_str());
    #endif	

    double iw = HPDF_Image_GetWidth (background);
    double ih = HPDF_Image_GetHeight (background);
    
    
    /*
     * Variables Needed for rendering in right positions
     * 
     */
    
    if(debug_mode) cout << "Initializing needed Variables Needed for rendering in right positions...!" << endl;
    
    // From Config File
    int certificateWidth = mainConfig.certificateWidth;
    int certificateHeight = mainConfig.certificateHeight;
    int textCorePosX = mainConfig.textCorePosX;
    int textCorePosY = mainConfig.textCorePosY;
    
    int currentCertificateX = 0;
    int currentCertificateY = 2; 
    int currentPositionX = 0;
    int currentPositionY = 0;
    int curentPage = 0;
    int signature_width = 100;
    int standardTextSize = 8;
    int textCoreX = 0;
    int textCoreY = 0;
    int certificateMarginM = 0; //(mainConfig.certificateMarginsB - mainConfig.certificateMarginsA ) / 2; 
    int midpoint = 0;
    int signaturePosX = 0, signaturePosY = 0;

    
    /*
    * Reading Names & Dates from file
    * 
    */
    
    // Population variables
    string line;
    stack<string> lines;
    
    // Input stream for file
    ifstream inputText (intputFileName.c_str());
    int linesIgnored = 0;
    
    // If can open read & populate lines...
    if (inputText.is_open())
    {
        while ( inputText.good() )
        {
            getline (inputText,line);
            if(debug_mode) cout << line << endl;
            if(line.length() > 10) {
                if(usersSetLimit==0) lines.push(line);
                else {
                    if(lines.size() < usersSetLimit) lines.push(line);
                    else {
                    linesIgnored++; 
                    if(debug_mode) cout << line << " has been ignored!" << endl; }
                }
            }
        }
        inputText.close();
    }

    else {
        cerr << "Could not open: " << intputFileName << endl;
        return 0;
    }
    
    if(debug_mode) cout << "It looks like we have " << lines.size() << " objects to work with!" << endl;
    
    if(debug_mode) cout << "Initializing page population ...";
    
    int numberOfPages = ceil( (float) (usersSetLimit==0?lines.size():usersSetLimit) / 6);
    vector<string> fields;
    vector<string>::iterator fit;
    
    if(debug_mode) cout <<  "looks like we need " << numberOfPages << " pages." << endl;

    for(int currentPage = 0; currentPage < numberOfPages; currentPage++) {

    // Add page & Set Font size (Setting up the next page)...
        
    
    if(debug_mode) cout << "Rendering initial background...!" << endl;
    /* Draw image to the canvas. (normal-mode with actual size.)*/
    
        page = HPDF_AddPage(pdf);
        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_LANDSCAPE);
        dst = HPDF_Page_CreateDestination (page);
        HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
        HPDF_SetOpenAction(pdf, dst);
        HPDF_Page_SetFontAndSize (page, font, 20);
        if(debug_render) HPDF_Page_DrawImage (page, background, 0, 0, iw, ih);

        for(currentCertificateY = 2; currentCertificateY > 0; currentCertificateY--)
        for(currentCertificateX = 0; currentCertificateX < 3; currentCertificateX++) {

            if(!lines.empty()) {
                
                string currentLine = lines.top();
                string temp;
                size_t position;
                
                if(debug_mode) cout << "Processing line: " << currentLine << endl;
                
                while (currentLine.find(",", 0) != string::npos)
                { //does the string a comma in it?
                    position = currentLine.find(",", 0); //store the position of the delimiter
                    temp = currentLine.substr(0, position);      //get the token
                    currentLine.erase(0, position + 1);          //erase it from the source 
                    fields.push_back(temp);                //and put it into the array
                }
                
                fields.push_back(currentLine); // add the rest of the line
                lines.pop(); 
                
            } else continue;

            if(debug_mode) cout << "current field vector contains "<<fields.size()<<" elements:" << endl;
            for (fit=fields.begin(); fit<fields.end(); fit++)
                if(debug_mode) cout << " " << *fit << "" << endl;

            currentPositionX = currentCertificateX * certificateWidth;
            currentPositionY = currentCertificateY * certificateHeight;

            // Print Just the Crosses of core document positions
            if(debug_mode) cout << "CurrentPositionXY: " << currentPositionX << ", " << currentPositionY << endl;
            print_description(page, currentPositionX, currentPositionY, "", standardTextSize);

            // Now lets go for the core of the text
            textCoreX = currentPositionX + textCorePosX;
            textCoreY = currentPositionY + textCorePosY;

            // Manager Code
            print_description(page, textCoreX + mainConfig.textManagerNamePosX, 
                                    textCoreY + mainConfig.textManagerNamePosY,
                                    mainConfig._managerCode.c_str(),
                                    mainConfig._managerCode_fontsize);
            // URL Address
            print_description(page, textCoreX + mainConfig.textUrlAddressPosX, 
                                    textCoreY + mainConfig.textUrlAddressPosY,
                                    mainConfig._urlAddress.c_str(),
                                    mainConfig._urlAddress_fontsize);
            // Company Name
            print_description(page, textCoreX + mainConfig.textCompanyNamePosX, 
                                    textCoreY + mainConfig.textCompanyNamePosY,
                                    mainConfig._companyName.c_str(),
                                    mainConfig._companyName_fontsize);
            
            // Phone Number
            print_description(page, textCoreX + mainConfig.textPhoneNumberPosX, 
                                    textCoreY + mainConfig.textPhoneNumberPosY,
                                    mainConfig._phoneNumber.c_str(),
                                    mainConfig._phoneNumber_fontsize);
            
            // License Number
            print_description(page, textCoreX + mainConfig.textLicenseNumberPosX, 
                                    textCoreY + mainConfig.textLicenseNumberPosY,
                                    mainConfig._licenseNumber.c_str(),
                                    mainConfig._licenseNumber_fontsize);
            
            // Issuance Date
            print_description(page, textCoreX + mainConfig.textIssuedDatePosX, 
                                    textCoreY + mainConfig.textIssuedDatePosY,
                                    string("Issued: " + fields[1]).c_str(), 
                                    mainConfig.textIssuedDateSize);
            
            // Expiration Date
            print_description(page, textCoreX + mainConfig.textExpirationDatePosX, 
                                    textCoreY + mainConfig.textExpirationDatePosY,
                                    string("Expires: " + fields[1]).c_str(), 
                                    mainConfig.textExpirationDateSize);
            
            
            // Set Page Font Size (just for precaution & text width calc!
            HPDF_Page_SetFontAndSize (page, HPDF_Page_GetCurrentFont (page), mainConfig.textNameSize);
            
            // Find the middle point between two points
            certificateMarginM = (mainConfig.certificateMarginsB / 2 ) + mainConfig.certificateMarginsA; 
            
            midpoint = (certificateMarginM - ((certificateWidth / 2))) / 2;
            // If textNamePosX == 0 then calculate center automatically
            if(mainConfig.textNamePosX == 0) 
                    // mainConfig.textNamePosX = ( ((certificateWidth)/2) - HPDF_Page_TextWidth (page, fields[0].c_str()));
                    mainConfig.textNamePosX = (certificateMarginM - (HPDF_Page_TextWidth (page, fields[0].c_str()) / 2));
            
            
            // Name
            print_description(page, 
                              currentPositionX + ((certificateWidth / 2) - (HPDF_Page_TextWidth (page, fields[0].c_str()) / 2)),
                              currentPositionY + mainConfig.textNamePosY,
                              fields[0].c_str(),
                              mainConfig.textNameSize);
            
            if(debug_mode) cout << "Margins[a,b,m]: " << mainConfig.certificateMarginsA << ","
                                       << mainConfig.certificateMarginsB << ","
                                       << certificateMarginM << endl;
            
            if(debug_mode) cout << "Text Width for " << fields[0].c_str() << ", is: " 
                                      << HPDF_Page_TextWidth (page, fields[0].c_str())
                                      << " /2 ="
                                      << (HPDF_Page_TextWidth (page, fields[0].c_str()) / 2)
                                      << ", drawn @: "
                                      << mainConfig.textNamePosX
                                      << ", should be:"
                                      << (certificateMarginM - (HPDF_Page_TextWidth (page, fields[0].c_str()) / 2))
                                      << endl;
            // Margin A
            print_description(page, 
                              currentPositionX + mainConfig.certificateMarginsA,
                              currentPositionY + mainConfig.certificateMarginsY,
                              "",
                              8);
            // Margin B  
            print_description(page, 
                              currentPositionX + mainConfig.certificateMarginsB,
                              currentPositionY + mainConfig.certificateMarginsY,
                              "",
                              8);
            // Margin M
            print_description(page, 
                              currentPositionX + certificateMarginM,
                              currentPositionY + mainConfig.certificateMarginsY,
                              "",
                              8);
            // NameX 
            print_description(page, 
                              currentPositionX + (certificateWidth / 2),
                              currentPositionY + mainConfig.certificateMarginsY,
                              "",
                              8);
            
            
            signaturePosX = mainConfig.imageSignaturePosX == 0 ?
                            ((certificateWidth / 2) - (signature_width / 2)) : //yes
                            mainConfig.imageSignaturePosX; // NO
            
            signaturePosY = mainConfig.imageSignaturePosY == 0 ?
                            ( certificateHeight - 30) : //yes
                            mainConfig.imageSignaturePosY; // NO
            
            // Signature Fixed Width & Height: 100
            print_image(page,
                        currentPositionX + signaturePosX, 
                        currentPositionY + signaturePosY,
                        signature, signature_width, signature_width);

            fields.clear();
        }
    }
    
    if(debug_mode) cout << "Saving to file...!" << endl;
	
    /* save the document to a file */
    HPDF_SaveToFile (pdf, outputPdfFile.c_str());

    if(debug_mode) cout << "Clearing pdf document memory...!" << endl;
    /* clean up */
    HPDF_Free (pdf);

    if(debug_mode) cout << "Exiting, thanks! :)" << endl;
    return 0;
}

#else

int main()
{
    cout <<"WARNING: if you want to run this program, \n" << "make libhpdf with HPDF_USE_PNGLIB option.\n";
    return 0;
}

#endif /* HPDF_NOPNGLIB */

