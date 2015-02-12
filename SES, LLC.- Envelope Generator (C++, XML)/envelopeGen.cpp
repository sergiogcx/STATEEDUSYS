/*
 * << Certificate Generator >> -- envelopeGen.cpp
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

#include "envelopeConfig.h"

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

void print_square (HPDF_Page page, int pageadj, foobar bar, foobar_color color) {
    /* Draw Rectangle */
    HPDF_Page_SetRGBFill (page, color.r, color.g, color.b);
    HPDF_Page_Rectangle(page, bar.x, pageadj - bar.y, bar.w, bar.h);
    HPDF_Page_Fill (page);
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
    envelopeConfig envConfig;
    
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
    
    // Font Variables
    HPDF_Font detail_font;
    string detail_font_name;
    
    
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
    envConfig.setDebugMode(debug_mode);
    envConfig.loadXmlFile(config_file);    
    
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
    
    
    cout << "Checking if font exists" << endl;
    if(!doesFileExist(envConfig._font.c_str()))  {
        cerr << "Font file does not exist!: " << envConfig._font << endl;
        cerr << "Shutting down!" << endl;
        return 0;
    }
    
    detail_font_name = HPDF_LoadTTFontFromFile (pdf, envConfig._font.c_str(), HPDF_FALSE);
    font = HPDF_GetFont (pdf, detail_font_name.c_str(), NULL);
    
    /*
     * Variables Needed for rendering in right positions
     * 
     */
    
    if(debug_mode) cout << "Loading background & signature images...!" << endl;
    
    HPDF_Image logo;
    HPDF_Image mailicon;

    /* load image file. */
    #ifndef __WIN32__
    logo = HPDF_LoadPngImageFromFile (pdf, envConfig._logo.c_str());
    #else
    logo = HPDF_LoadPngImageFromFile (pdf, envConfig._logo.c_str());
    #endif

    /* load image file. */
    #ifndef __WIN32__
    mailicon = HPDF_LoadPngImageFromFile (pdf, envConfig._mail.c_str());
    #else
    mailicon = HPDF_LoadPngImageFromFile (pdf, envConfig._mail.c_str());
    #endif	

    
    
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
    
    int numberOfPages = lines.size();
    vector<string> fields;
    vector<string>::iterator fit;
    
    vector<string> address;
    vector<string>::iterator ait;
    
    if(debug_mode) cout <<  "looks like we need " << numberOfPages << " envelopes." << endl;

    for(int currentPage = 0; currentPage < numberOfPages; currentPage++) {

    // Add page & Set Font size (Setting up the next page)...
        
    
    if(debug_mode) cout << "Rendering initial background...!" << endl;
    /* Draw image to the canvas. (normal-mode with actual size.)*/
    
        page = HPDF_AddPage(pdf);
        
        // HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_COMM10, HPDF_PAGE_LANDSCAPE);
        HPDF_Page_SetHeight  (page, envConfig._pageHeight);
        HPDF_Page_SetWidth  (page, envConfig._pageWidth);
        
        // Page Destination
        dst = HPDF_Page_CreateDestination (page);
        HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
        HPDF_SetOpenAction(pdf, dst);
        HPDF_Page_SetFontAndSize (page, font, 20);
        
        
        
        
        
        if(!lines.empty()) {

            // Take the one at the top
            string currentLine = lines.top();
            string temp;
            size_t position;

            if(debug_mode) cout << "Processing line: " << currentLine << endl;

            while (currentLine.find("|", 0) != string::npos)
            { //does the string a comma in it?
                position = currentLine.find("|", 0); //store the position of the delimiter
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
         
        
        if(debug_mode) cout << "The last element of fields is: " << fields.back() << endl;
                
        if(!fields.empty()) {

            // Take the one at the top
            string address_string = fields.back();
            string temp_s;
            size_t position_a;

            if(debug_mode) cout << "Processing address: " << address_string << endl;

            while (address_string.find("%", 0) != string::npos)
            { //does the string a comma in it?
                position_a = address_string.find("%", 0); //store the position of the delimiter
                temp_s = address_string.substr(0, position_a);      //get the token
                address_string.erase(0, position_a + 1);          //erase it from the source 
                address.push_back(temp_s);                //and put it into the array
            }
            
            address.push_back(address_string);

        } else continue;
        
                
        if(debug_mode) cout << "current address vector contains "<<fields.size()<<" elements:" << endl;
        for (fit=address.begin(); fit<address.end(); fit++)
            if(debug_mode) cout << " " << *fit << "" << endl;
        
        if(envConfig._logo_pos_w == 0) envConfig._logo_pos_w = HPDF_Image_GetWidth (logo);
        if(envConfig._logo_pos_h == 0) envConfig._logo_pos_h = HPDF_Image_GetHeight (logo);
            
        // Signature Fixed Width & Height: 100
        print_image(page,
                    envConfig._logo_pos_x, 
                    envConfig._pageHeight - envConfig._logo_pos_y,
                    logo, 
                    envConfig._logo_pos_w,
                    envConfig._logo_pos_h);
        
        // First Foo Bar
        print_square(page, envConfig._pageHeight, envConfig._foobar_a, envConfig._foobar_color);
        
        // Title
        print_description(page, 
                            envConfig._title_x,
                            envConfig._pageHeight - envConfig._title_y,
                            envConfig._title.c_str(),
                            envConfig._title_size);
        
        // Company Name
        print_description(page, 
                            envConfig._company_x,
                            envConfig._pageHeight - envConfig._company_y,
                            envConfig._company.c_str(),
                            envConfig._company_size);
        
        // Company Address
        print_description(page, 
                            envConfig._company_address_x,
                            envConfig._pageHeight - envConfig._company_address_y,
                            envConfig._company_address.c_str(),
                            envConfig._company_address_size);
        
        // Company Address CSZ
        print_description(page, 
                            envConfig._company_address_csz_x,
                            envConfig._pageHeight - envConfig._company_address_csz_y,
                            envConfig._company_address_csz.c_str(),
                            envConfig._company_address_csz_size);
        
        // Company Address Phone
        print_description(page, 
                            envConfig._company_phone_x,
                            envConfig._pageHeight - envConfig._company_phone_y,
                            envConfig._company_phone.c_str(),
                            envConfig._company_phone_size);
        
        
        // Signature Fixed Width & Height: 100
        print_image(page,
                    envConfig._mail_pos_x, 
                    envConfig._pageHeight - envConfig._mail_pos_y,
                    mailicon, 
                    envConfig._mail_pos_w,
                    envConfig._mail_pos_h);
        
        // First Foo Bar
        print_square(page, envConfig._pageHeight, envConfig._foobar_b, envConfig._foobar_color);
        
        
        // User's Name
        print_description(page, 
                            envConfig._users_name_x,
                            envConfig._pageHeight - envConfig._users_name_y,
                            fields[0].c_str(),
                            envConfig._users_name_size);
        
        // User's Address
        print_description(page, 
                            envConfig._users_address_x,
                            envConfig._pageHeight - envConfig._users_address_y,
                            address[0].c_str(),
                            envConfig._users_address_size);
        
        // User's CSZ
        print_description(page, 
                            envConfig._users_address_csz_x,
                            envConfig._pageHeight - envConfig._users_address_csz_y,
                            address[1].c_str(),
                            envConfig._users_address_csz_size);

        fields.clear();
        address.clear();
        
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

