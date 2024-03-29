// test_rtcm.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>

static void extract_coordinate(const char* fname)
{
    FILE* fLOG = fopen(fname, "rt");

    if (fLOG ==NULL) return;

    char buffer[255] = { 0 };

    double xyz[3] = { 0 };
    double rms_xyz[3] = { 0 };
    int idx[3] = { 0 };
    double mp12 = 0;
    double mp21 = 0;
    double mp15 = 0;
    double mp51 = 0;
    double mp17 = 0;
    double mp71 = 0;
    int iod_slip = 0;
    int mp_slip = 0;
    int oslps = 0;
    int numobs = 0;
    int numepoch = 0;
    int expepoch = 0;
    char oslip_str[50] = { 0 };
    char name[255] = { 0 };
    char datestr[255] = { 0 };
    strcpy(buffer, fname);
    char* temp = strrchr(buffer, '-');
    if (temp) strcpy(name, temp + 1);
    temp = strrchr(name, '.');
    if (temp) temp[0] = '\0';
    strcpy(datestr, fname);
    temp = strrchr(datestr, '-');
    if (temp) temp[0] = '\0';
    while (fLOG && !feof(fLOG))
    {
        fgets(buffer, sizeof(buffer), fLOG);
        temp = strrchr(buffer, '\n');
        if (temp) temp[0] = '\0';
        temp = strrchr(buffer, '\r');
        if (temp) temp[0] = '\0';
        if (strstr(buffer, "Moving average MP12") && strlen(buffer) >= 34)
        {
            mp12 = atof(buffer + 25);
        }
        if (strstr(buffer, "Moving average MP21") && strlen(buffer) >= 34)
        {
            mp21 = atof(buffer + 25);
        }
        if (strstr(buffer, "Moving average MP15") && strlen(buffer) >= 34)
        {
            mp15 = atof(buffer + 25);
        }
        if (strstr(buffer, "Moving average MP51") && strlen(buffer) >= 34)
        {
            mp51 = atof(buffer + 25);
        }
        if (strstr(buffer, "Moving average MP17") && strlen(buffer) >= 34)
        {
            mp17 = atof(buffer + 25);
        }
        if (strstr(buffer, "Moving average MP71") && strlen(buffer) >= 34)
        {
            mp71 = atof(buffer + 25);
        }
        if (strstr(buffer, "Complete observations") && strlen(buffer) >= 32)
        {
            numobs = atof(buffer + 25);
        }
        if (strstr(buffer, "Poss. # of obs epochs") && strlen(buffer) >= 32)
        {
            expepoch = atof(buffer + 25);
        }
        if (strstr(buffer, "Epochs w/ observations") && strlen(buffer) >= 32)
        {
            numepoch = atof(buffer + 25);
        }
        if (strstr(buffer, "SUM") && buffer[0] == 'S' && buffer[1] == 'U' && buffer[2] == 'M' && strlen(buffer) >= 80)
        {
            oslps = atof(buffer + 74);
            strcpy(oslip_str, buffer + 62);
        }
        if (strstr(buffer, "IOD slips") && strlen(buffer) >= 32)
        {
            iod_slip += atof(buffer + 25);
        }
        if (strstr(buffer, "IOD or MP slips") && strlen(buffer) >= 32)
        {
            mp_slip += atof(buffer + 25);
        }
    }
    if (numepoch>0)
    {
        printf("%s,%s,%6i,%6i,%6i,%6i,%6i,%6i,%10.6f,%10.6f,%10.6f,%10.6f,%10.6f,%10.6f,%s,%s\n", name, datestr, expepoch, numepoch, numobs, iod_slip, mp_slip, oslps, mp12, mp21, mp15, mp51, mp17, mp71, fname, oslip_str);
        FILE* fOUT = fopen("teqc_result.csv", "a");
        if (fOUT) fprintf(fOUT,"%s,%s,%6i,%6i,%6i,%6i,%6i,%6i,%10.6f,%10.6f,%10.6f,%10.6f,%10.6f,%10.6f,%s,%s\n", name, datestr, expepoch, numepoch, numobs, iod_slip, mp_slip, oslps, mp12, mp21, mp15, mp51, mp17, mp71, fname, oslip_str);
        if (fOUT) fclose(fOUT);
    }
    if (fLOG) fclose(fLOG);
    return;
}

int main(int argc, const char* argv[])
{
    if (argc>1)
        extract_coordinate(argv[1]);
    return 0;
}

