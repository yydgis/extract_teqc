// test_rtcm.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

struct teqc_t
{
    int epoch_exp;
    int epoch_obs;
    int iod_slips;
    int mp_slips;
    int oslips;
    double mp12;
    double mp21;
    double mp15;
    double mp51;
    double mp16;
    double mp61;
    double mp17;
    double mp71;
    double mp18;
    double mp81;
    std::string rcv_type;
};

struct rcv_teqc_t
{
    std::string name;
    std::map<int, teqc_t> teqc;
    int total_exp;
    int total_obs;
    int oslips;
    double rate;
    double mp12;
    double mp21;
    double mp15;
    double mp51;
    double mp16;
    double mp61;
    double mp17;
    double mp71;
    double mp18;
    double mp81;
    std::string rcv_type;
    int type;
};

#define MAXFIELD 100

static int parse_fields(char* const buffer, char** val)
{
    char* p, * q;
    int n = 0;

    /* parse fields */
    for (p = buffer; *p && n < MAXFIELD; p = q + 1) {
        if (p == NULL) break;
        if ((q = strchr(p, ',')) || (q = strchr(p, '*')) || (q = strchr(p, '\n')) || (q = strchr(p, '\r'))) {
            val[n++] = p; *q = '\0';
        }
        else break;
    }
    if (p) val[n++] = p;
    return n;
}

static void set_output_file_name(const char* fname, const char* key, char* outfname)
{
    char filename[255] = { 0 }, outfilename[255] = { 0 };
    strcpy(filename, fname);
    char* temp = strrchr(filename, '.');
    if (temp) temp[0] = '\0';
    sprintf(outfname, "%s-%s", filename, key);
}


static FILE* set_output_file(const char* fname, const char* key)
{
    char filename[255] = { 0 };
    set_output_file_name(fname, key, filename);
    return fopen(filename, "w");
}

static std::map<std::string, std::vector< rcv_teqc_t> > mRcv;

static void sum_teqc(const char* fname)
{
    FILE* fLOG = fopen(fname, "rt");

    if (fLOG ==NULL) return;


    char buffer[255] = { 0 };
    char* val[MAXFIELD];

    int line_index = 0;

    while (fLOG && !feof(fLOG))
    {
        fgets(buffer, sizeof(buffer), fLOG);
        ++line_index;
        if (line_index < 2) continue;
        int num = parse_fields(buffer, val);
        if (num < 21) continue;

        std::string name = std::string(val[0]);
        std::string date = std::string(val[1]);

        teqc_t teqc = { 0 };

        int hour = atoi(val[2]);
        teqc.epoch_exp = atoi(val[3]);
        teqc.epoch_obs = atoi(val[4]);
        teqc.iod_slips = atoi(val[7]);
        teqc.mp_slips = atoi(val[8]);
        teqc.oslips = atoi(val[9]);
        teqc.mp12 = atof(val[10]);
        teqc.mp21 = atof(val[11]);
        teqc.mp15 = atof(val[12]);
        teqc.mp51 = atof(val[13]);
        teqc.mp16 = atof(val[14]);
        teqc.mp61 = atof(val[15]);
        teqc.mp17 = atof(val[16]);
        teqc.mp71 = atof(val[17]);
        teqc.mp18 = atof(val[18]);
        teqc.mp81 = atof(val[19]);
        teqc.rcv_type = std::string(val[20]);

        

        int is_found = 0;
        for (std::vector<rcv_teqc_t>::iterator pRcv = mRcv[date].begin(); pRcv != mRcv[date].end(); ++pRcv)
        {
            if (pRcv->name == name)
            {
                is_found = 1;
                pRcv->teqc[hour] = teqc;
            }
        }
        if (!is_found)
        {
            rcv_teqc_t rcv_teqc;
            rcv_teqc.name = name;
            rcv_teqc.teqc[hour] = teqc;
            mRcv[date].push_back(rcv_teqc);
        }
    }
    if (fLOG) fclose(fLOG);
    return;
}

struct coord_t
{
    std::string country;
    double lat;
    double lon;
};

std::map<std::string, coord_t> mCoords;

static int read_coord_file(const char* fname)
{
    FILE* fLOG = fopen(fname, "r");
    char buffer[255] = { 0 };
    unsigned long numofline = 0;
    char* val[MAXFIELD];
    while (fLOG && !feof(fLOG) && fgets(buffer, sizeof(buffer), fLOG))
    {
        ++numofline;
        if (numofline < 2) continue;
        int num = parse_fields(buffer, val);
        if (num < 4) continue;
        std::string name = std::string(val[0]);
        double lat = atof(val[1]);
        double lon = atof(val[2]);
        if (name.size() > 0 && fabs(lat) > 0.001 && fabs(lon) > 0.001)
        {
            mCoords[name].lat = lat;
            mCoords[name].lon = lon;
            mCoords[name].country = val[3];
        }
    }
    if (fLOG) fclose(fLOG);
    return mCoords.size();
}

struct relay_t
{
    int strType;
    std::string status;
    std::string reason;
    std::string host;
    int port;
    std::string username;
    std::string password;
    std::string mountpoint;
    int isUsed;
    int isEph;
    double xyz_spp[3];
    double xyz_ppp[3];
    std::string coordSystemName;
    double xyz_offset[3];
//    name, svrType(0: ALL; 1: L1l5), status, reason, host, port, username, password, mountpoint, isUsed(1: RTK Service), eph(1: Send eph data), SPP, PPP displacement detection, PPP, coordinate system name, offset
//       D8BC387F85F1, 0, ACTIVE, , 52.8.236.207, 2201, admin, admin, D8BC387F85F1, 1, 0, x: 1363855.824; y: -4651430.175; z: 4131511.714, 1, x : 1363856.432; y: -4651431.709; z: 4131511.104, NAD83(2011) (2010.0), x : 0; y : 0; z: 0
};

std::map<std::string, relay_t> mRelays;

static int read_relay_file(const char* fname)
{
    FILE* fLOG = fopen(fname, "r");
    char buffer[255] = { 0 };
    unsigned long numofline = 0;
    char* val[MAXFIELD];
    while (fLOG && !feof(fLOG) && fgets(buffer, sizeof(buffer), fLOG))
    {
        ++numofline;
        if (numofline < 2) continue;
        int num = parse_fields(buffer, val);
        if (num <16) continue;
        std::string name = std::string(val[0]);
        int svrType = atoi(val[1]);
        std::string status = std::string(val[2]);
        std::string reason = std::string(val[3]);
        std::string mount = std::string(val[8]);
        std::string xyz_ppp_str = std::string(val[13]);
        std::string coordSystemName = std::string(val[14]);
        std::string xyz_off_str = std::string(val[15]);
        strcpy(buffer, xyz_ppp_str.c_str());
        char *found = strstr(buffer, "x:");
        if (found)
        {
            found[0] = ' ';
            found[1] = ' ';
        }
        found = strstr(buffer, "y:");
        if (found)
        {
            found[0] = ' ';
            found[1] = ',';
        }
        found = strstr(buffer, "z:");
        if (found)
        {
            found[0] = ' ';
            found[1] = ',';
        }
        num = parse_fields(buffer, val);
        double xyz_ppp[3] = { 0 };
        if (num > 2)
        {
            xyz_ppp[0] = atof(val[0]);
            xyz_ppp[1] = atof(val[1]);
            xyz_ppp[2] = atof(val[2]);
        }
        strcpy(buffer, xyz_off_str.c_str());
        found = strstr(buffer, "x:");
        if (found)
        {
            found[0] = ' ';
            found[1] = ' ';
        }
        found = strstr(buffer, "y:");
        if (found)
        {
            found[0] = ' ';
            found[1] = ',';
        }
        found = strstr(buffer, "z:");
        if (found)
        {
            found[0] = ' ';
            found[1] = ',';
        }
        num = parse_fields(buffer, val);
        double xyz_off[3] = { 0 };
        if (num > 2)
        {
            xyz_off[0] = atof(val[0]);
            xyz_off[1] = atof(val[1]);
            xyz_off[2] = atof(val[2]);
        }
        if (name.size() > 0)
        {
            mRelays[name].mountpoint = mount;
            mRelays[name].coordSystemName = coordSystemName;
            mRelays[name].xyz_offset[0] = xyz_off[0];
            mRelays[name].xyz_offset[1] = xyz_off[1];
            mRelays[name].xyz_offset[2] = xyz_off[2];
            mRelays[name].xyz_ppp[0] = xyz_ppp[0];
            mRelays[name].xyz_ppp[1] = xyz_ppp[1];
            mRelays[name].xyz_ppp[2] = xyz_ppp[2];
            if (name != mount)
            {
                printf("%s,%s mountpoint does not match\n", name.c_str(), mount.c_str());
            }
        }
    }
    if (fLOG) fclose(fLOG);
    return mRelays.size();
}

std::vector<std::string> stations;

static int read_station_list(const char* fname)
{
    FILE* fLOG = fopen(fname, "r");
    char buffer[255] = { 0 };
    int data = 0;
    unsigned long numofread = 0;
    unsigned long numofline = 0;
    char* val[MAXFIELD];
    while (fLOG && !feof(fLOG) && (data = fgetc(fLOG)) != EOF)
    {
        if (!numofread)
            memset(buffer, 0, sizeof(buffer));
        if (data == '\r' || data == '\n')
        {
            ++numofline;
            numofread = 0;
        }
        else
        {
            buffer[numofread++] = data;
            continue;
        }
        if (numofline < 2) continue;
        int num = parse_fields(buffer, val);
        if (num < 1) continue;
        std::string name = std::string(val[0]);
        name.erase(name.find_last_not_of(" \n\r\t") + 1);
        if (name.size() > 0 && std::find(stations.begin(), stations.end(), name)==stations.end())
        {
            stations.push_back(name);
        }
    }
    if (fLOG) fclose(fLOG);
    return stations.size();
}
static int check_na_coordinate_system()
{
    int num = 0;
    FILE* fBAD = fopen("nomatch.csv", "w");
    FILE* fNA = fopen("na.csv", "w");
    FILE* fOT = fopen("others.csv", "w");
    for (std::map<std::string, relay_t>::iterator pRelay = mRelays.begin(); pRelay != mRelays.end(); ++pRelay)
    {
        std::map<std::string, coord_t>::iterator pCoord = mCoords.find(pRelay->first);
        if (pCoord == mCoords.end())
        {
            if (fBAD) fprintf(fBAD,"%s,%s,%14.4f,%14.4f,%14.4f,%10.4f,%10.4f,%10.4f cannot find a match\n", pRelay->first.c_str(), pRelay->second.coordSystemName.c_str(), pRelay->second.xyz_ppp[0], pRelay->second.xyz_ppp[1], pRelay->second.xyz_ppp[2], pRelay->second.xyz_offset[0], pRelay->second.xyz_offset[1], pRelay->second.xyz_offset[2]);
        }
        else
        {
            if (pCoord->second.country.find("USA") != std::string::npos || pCoord->second.country.find("CAN") != std::string::npos)
            {
                if (pRelay->second.coordSystemName.find("NAD") == std::string::npos)
                {
                    printf("%s,%s,%14.4f,%14.4f,%14.4f,%10.4f,%10.4f,%10.4f,%s\n", pRelay->first.c_str(), pRelay->second.coordSystemName.c_str(), pRelay->second.xyz_ppp[0], pRelay->second.xyz_ppp[1], pRelay->second.xyz_ppp[2], pRelay->second.xyz_offset[0], pRelay->second.xyz_offset[1], pRelay->second.xyz_offset[2], pCoord->second.country.c_str());
                }
                if (fNA) fprintf(fNA, "%s,%s,%14.4f,%14.4f,%14.4f,%10.4f,%10.4f,%10.4f,%s\n", pRelay->first.c_str(), pRelay->second.coordSystemName.c_str(), pRelay->second.xyz_ppp[0], pRelay->second.xyz_ppp[1], pRelay->second.xyz_ppp[2], pRelay->second.xyz_offset[0], pRelay->second.xyz_offset[1], pRelay->second.xyz_offset[2], pCoord->second.country.c_str());
            }
            else
            {
                if (pRelay->second.coordSystemName.find("NAD") != std::string::npos)
                {
                    printf("%s,%s,%14.4f,%14.4f,%14.4f,%10.4f,%10.4f,%10.4f,%s\n", pRelay->first.c_str(), pRelay->second.coordSystemName.c_str(), pRelay->second.xyz_ppp[0], pRelay->second.xyz_ppp[1], pRelay->second.xyz_ppp[2], pRelay->second.xyz_offset[0], pRelay->second.xyz_offset[1], pRelay->second.xyz_offset[2], pCoord->second.country.c_str());
                }
                if (fOT) fprintf(fOT, "%s,%s,%14.4f,%14.4f,%14.4f,%10.4f,%10.4f,%10.4f,%s\n", pRelay->first.c_str(), pRelay->second.coordSystemName.c_str(), pRelay->second.xyz_ppp[0], pRelay->second.xyz_ppp[1], pRelay->second.xyz_ppp[2], pRelay->second.xyz_offset[0], pRelay->second.xyz_offset[1], pRelay->second.xyz_offset[2], pCoord->second.country.c_str());
            }
        }
    }
    if (fBAD) fclose(fBAD);
    if (fNA) fclose(fNA);
    if (fOT) fclose(fOT);
    return 0;
}
int main(int argc, const char* argv[])
{
    read_relay_file("relay.csv");
    read_coord_file("all_online.csv");
    read_station_list("stations.csv");
    check_na_coordinate_system();
    for (int i = 1; i < argc; ++i)
        sum_teqc(argv[i]);
    for (std::map<std::string, std::vector<rcv_teqc_t>>::iterator pRcv = mRcv.begin(); pRcv != mRcv.end(); ++pRcv)
    {
        unsigned long numofp40 = 0;
        unsigned long numof980 = 0;
        unsigned long numof335 = 0;
        unsigned long numofigs = 0;
        for (std::vector<rcv_teqc_t>::iterator pStn = pRcv->second.begin(); pStn != pRcv->second.end(); ++pStn)
        {
            int total_exp = 0;
            int total_obs = 0;
            int oslips = 0;
            double mp12 = 0;
            double mp21 = 0;
            double mp15 = 0;
            double mp51 = 0;
            double mp16 = 0;
            double mp61 = 0;
            double mp17 = 0;
            double mp71 = 0;
            double mp18 = 0;
            double mp81 = 0;
            int n = 0;
            std::string rcv_type;
            for (std::map<int, teqc_t>::iterator pTeqc = pStn->teqc.begin(); pTeqc != pStn->teqc.end(); ++pTeqc)
            {
                total_exp += pTeqc->second.epoch_exp;
                total_obs += pTeqc->second.epoch_obs;
                oslips += pTeqc->second.oslips;
                mp12 += pTeqc->second.mp12;
                mp21 += pTeqc->second.mp21;
                mp15 += pTeqc->second.mp15;
                mp51 += pTeqc->second.mp51;
                mp16 += pTeqc->second.mp16;
                mp61 += pTeqc->second.mp61;
                mp17 += pTeqc->second.mp17;
                mp71 += pTeqc->second.mp71;
                mp18 += pTeqc->second.mp18;
                mp81 += pTeqc->second.mp81;
                rcv_type = pTeqc->second.rcv_type;
                n++;
            }
            if (n > 0)
            {
                oslips /= n;
                mp12 /= n;
                mp21 /= n;
                mp15 /= n;
                mp51 /= n;
                mp16 /= n;
                mp61 /= n;
                mp17 /= n;
                mp71 /= n;
                mp18 /= n;
                mp81 /= n;
            }
            pStn->rate = (total_exp > 0 ? total_obs * 100.0 / total_exp : 0);
            pStn->total_exp = total_exp;
            pStn->total_obs = total_obs;
            pStn->oslips = oslips;
            pStn->mp12 = mp12;
            pStn->mp21 = mp21;
            pStn->mp15 = mp15;
            pStn->mp51 = mp51;
            pStn->mp16 = mp16;
            pStn->mp61 = mp61;
            pStn->mp17 = mp17;
            pStn->mp71 = mp71;
            pStn->mp18 = mp18;
            pStn->mp81 = mp81;
            pStn->rcv_type = rcv_type;
            if (rcv_type.find("P40") != std::string::npos)
            {
                pStn->type = 1;
                ++numofp40;
            }
            else if (rcv_type.find("UM980") != std::string::npos)
            {
                pStn->type = 2;
                ++numof980;
            }
            else if (pStn->name.length()==4)
            {
                pStn->type = 0;
                ++numofigs;
            }
            else
            {
                pStn->type = 3;
                ++numof335;
            }
        }
        for (int i=0; i< pRcv->second.size(); ++i)
		{
			for (int j=i+1;j<pRcv->second.size();++j)
			{
                int is_swap = 0;
                if (pRcv->second[i].oslips < pRcv->second[j].oslips)
                {
                    is_swap = 1;
                }
                else if (pRcv->second[i].oslips == pRcv->second[j].oslips)
                {
                    if (pRcv->second[i].rate < pRcv->second[j].rate)
                    {
                        is_swap = 1;
                    }
                    else if (pRcv->second[i].rate == pRcv->second[j].rate)
                    {
                        if (pRcv->second[i].total_exp < pRcv->second[j].total_exp)
                            is_swap = 1;
                        else if (pRcv->second[i].total_exp == pRcv->second[j].total_exp)
                        {
                            is_swap = pRcv->second[i].total_obs < pRcv->second[j].total_obs;
                        }
                    }
                }
                if (is_swap)
				{
                    std::swap(pRcv->second[i], pRcv->second[j]);
				}
			}
		}
        std::string cur_date = pRcv->first;
        for (int i = 0; i < cur_date.size(); ++i)
        {
            if (cur_date[i] == '/')
                cur_date[i] = '-';
        }

        FILE *fLIST = stations.size()>0 ? set_output_file(cur_date.c_str(), "-list.csv") : NULL;
        if (fLIST) fprintf(fLIST, "SN,QC,epoch exp,epoch obs,availability,oslips,MP12,MP21,MP15,MP51,MP16,MP61,MP17,MP71,MP18,MP81,qc_counter,rcv type\n");
        int numofok = 0;
        for (std::vector<std::string>::iterator pList = stations.begin(); pList != stations.end(); ++pList)
        {
            int isOK = 0;
            for (std::vector<rcv_teqc_t>::iterator pStn = pRcv->second.begin(); pStn != pRcv->second.end(); ++pStn)
            {
                if (pStn->name == *pList)
                {
                    int qc_ok = 0;
                    if (pStn->oslips >= 4000 && pStn->mp12 <= 0.5 && pStn->mp21 <= 0.5 && pStn->mp12 > 0.0 && pStn->mp21 > 0.0 && pStn->total_obs >= (86400 * 0.99)) {
                        qc_ok = 1; numofok++;
                    }
                    if (fLIST) fprintf(fLIST, "%s,%i,%6i,%6i,%6.2f,%6i,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%i,%s,%s\n", pStn->name.c_str(), qc_ok, pStn->total_exp, pStn->total_obs, pStn->rate * 1.0, pStn->oslips, pStn->mp12, pStn->mp21, pStn->mp15, pStn->mp51, pStn->mp16, pStn->mp61, pStn->mp17, pStn->mp71, pStn->mp18, pStn->mp81, numofok, pRcv->first.c_str(), pStn->rcv_type.c_str());
                    isOK = 1;
                    break;
                }
            }
            if (!isOK)
            {
                if (fLIST) fprintf(fLIST, "%s,,,,,,,,,,,,,,,,,,\n", pList->c_str());
            }
        }
        if (fLIST) fclose(fLIST);

        FILE* fOUT_P40 = numofp40 > 0 ? set_output_file(cur_date.c_str(), "-P40.csv") : NULL;
        FILE* fOUT_980 = numof980 > 0 ? set_output_file(cur_date.c_str(), "-980.csv") : NULL;
        FILE* fOUT_335 = numof335 > 0 ? set_output_file(cur_date.c_str(), "-MTK.csv") : NULL;
        FILE* fOUT_IGS = numofigs > 0 ? set_output_file(cur_date.c_str(), "-IGS.csv") : NULL;
        if (fOUT_P40) fprintf(fOUT_P40, "SN,QC,latitude,longitude,country,epoch exp,epoch obs,availability,oslips,MP12,MP21,MP15,MP51,MP16,MP61,MP17,MP71,MP18,MP81,QC counter,date,rcv type\n");
        if (fOUT_980) fprintf(fOUT_980, "SN,QC,latitude,longitude,country,epoch exp,epoch obs,availability,oslips,MP12,MP21,MP15,MP51,MP16,MP61,MP17,MP71,MP18,MP81,QC counter,date,rcv type\n");
        if (fOUT_335) fprintf(fOUT_335, "SN,QC,latitude,longitude,country,epoch exp,epoch obs,availability,oslips,MP12,MP21,MP15,MP51,MP16,MP61,MP17,MP71,MP18,MP81,QC counter,date,rcv type\n");
        if (fOUT_IGS) fprintf(fOUT_IGS, "SN,QC,latitude,longitude,country,epoch exp,epoch obs,availability,oslips,MP12,MP21,MP15,MP51,MP16,MP61,MP17,MP71,MP18,MP81,QC counter,date,rcv type\n");
		int numofp40_ok = 0;
		int numof980_ok = 0;
		int numof335_ok = 0;
		int numofigs_ok = 0;
        for (std::vector<rcv_teqc_t>::iterator pStn = pRcv->second.begin(); pStn != pRcv->second.end(); ++pStn)
        {
			int is_ok = 0;
			if (pStn->type == 0 && pStn->oslips>=4000 && pStn->mp12 <=0.5 && pStn->mp21<=0.5 && pStn->mp12>0.0 && pStn->mp21 > 0.0 && pStn->total_obs >= (86400*0.99)) { ++numofp40_ok; is_ok = 1; }
			if (pStn->type == 1 && pStn->oslips>=4000 && pStn->mp12 <=0.5 && pStn->mp21<=0.5 && pStn->mp12>0.0 && pStn->mp21 > 0.0 && pStn->total_obs >= (86400*0.99)) { ++numof980_ok; is_ok = 1; }
			if (pStn->type == 2 && pStn->oslips>=4000 && pStn->mp15 <=0.5 && pStn->mp51<=0.5 && pStn->mp15>0.0 && pStn->mp51 > 0.0 && pStn->total_obs >= (86400*0.99)) { ++numof335_ok; is_ok = 1; }
			if (pStn->type == 3 && pStn->oslips>=4000 && pStn->mp12 <=0.5 && pStn->mp21<=0.5 && pStn->mp12>0.0 && pStn->mp21 > 0.0 && pStn->total_obs >= (86400*0.99)) { ++numofigs_ok; is_ok = 1; }

            if (pStn->type == 0 && fOUT_IGS) fprintf(fOUT_IGS, "%s,%i,%7.2f,%7.2f,%s,%6i,%6i,%6.2f,%6i,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%i,%s,%s\n", pStn->name.c_str(), is_ok, mCoords[pStn->name].lat, mCoords[pStn->name].lon, mCoords[pStn->name].country.c_str(), pStn->total_exp, pStn->total_obs, pStn->rate * 1.0, pStn->oslips, pStn->mp12, pStn->mp21, pStn->mp15, pStn->mp51, pStn->mp16, pStn->mp61, pStn->mp17, pStn->mp71, pStn->mp18, pStn->mp81, numofp40_ok, pRcv->first.c_str(), pStn->rcv_type.c_str());
            if (pStn->type == 1 && fOUT_P40) fprintf(fOUT_P40, "%s,%i,%7.2f,%7.2f,%s,%6i,%6i,%6.2f,%6i,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%i,%s,%s\n", pStn->name.c_str(), is_ok, mCoords[pStn->name].lat, mCoords[pStn->name].lon, mCoords[pStn->name].country.c_str(), pStn->total_exp, pStn->total_obs, pStn->rate * 1.0, pStn->oslips, pStn->mp12, pStn->mp21, pStn->mp15, pStn->mp51, pStn->mp16, pStn->mp61, pStn->mp17, pStn->mp71, pStn->mp18, pStn->mp81, numof980_ok, pRcv->first.c_str(), pStn->rcv_type.c_str());
            if (pStn->type == 2 && fOUT_980) fprintf(fOUT_980, "%s,%i,%7.2f,%7.2f,%s,%6i,%6i,%6.2f,%6i,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%i,%s,%s\n", pStn->name.c_str(), is_ok, mCoords[pStn->name].lat, mCoords[pStn->name].lon, mCoords[pStn->name].country.c_str(), pStn->total_exp, pStn->total_obs, pStn->rate * 1.0, pStn->oslips, pStn->mp12, pStn->mp21, pStn->mp15, pStn->mp51, pStn->mp16, pStn->mp61, pStn->mp17, pStn->mp71, pStn->mp18, pStn->mp81, numof335_ok, pRcv->first.c_str(), pStn->rcv_type.c_str());
            if (pStn->type == 3 && fOUT_335) fprintf(fOUT_335, "%s,%i,%7.2f,%7.2f,%s,%6i,%6i,%6.2f,%6i,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%i,%s,%s\n", pStn->name.c_str(), is_ok, mCoords[pStn->name].lat, mCoords[pStn->name].lon, mCoords[pStn->name].country.c_str(), pStn->total_exp, pStn->total_obs, pStn->rate * 1.0, pStn->oslips, pStn->mp12, pStn->mp21, pStn->mp15, pStn->mp51, pStn->mp16, pStn->mp61, pStn->mp17, pStn->mp71, pStn->mp18, pStn->mp81, numofigs_ok, pRcv->first.c_str(), pStn->rcv_type.c_str());
        }
        if (fOUT_P40) fclose(fOUT_P40);
        if (fOUT_980) fclose(fOUT_980);
        if (fOUT_335) fclose(fOUT_335);
        if (fOUT_IGS) fclose(fOUT_IGS);
    }
    return 0;
}

