// test_rtcm.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <string>

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
    int rate;
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

int main(int argc, const char* argv[])
{
    read_coord_file("all_online.csv");
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
            pStn->rate = (int)(total_exp > 0 ? total_obs * 100.0 / total_exp : 0);
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


        FILE* fOUT_P40 = numofp40 > 0 ? set_output_file(cur_date.c_str(), "-P40.csv") : NULL;
        FILE* fOUT_980 = numof980 > 0 ? set_output_file(cur_date.c_str(), "-980.csv") : NULL;
        FILE* fOUT_335 = numof335 > 0 ? set_output_file(cur_date.c_str(), "-MTK.csv") : NULL;
        FILE* fOUT_IGS = numofigs > 0 ? set_output_file(cur_date.c_str(), "-IGS.csv") : NULL;
        if (fOUT_P40) fprintf(fOUT_P40, "date,latitude,longitude,country,epoch exp, epoch obs, availability, oslips, MP12, MP21, MP15, MP51, MP16, MP61, MP17, MP71, MP18, MP81, SN, rcv type\n");
        if (fOUT_980) fprintf(fOUT_980, "date,latitude,longitude,country,epoch exp, epoch obs, availability, oslips, MP12, MP21, MP15, MP51, MP16, MP61, MP17, MP71, MP18, MP81, SN, rcv type\n");
        if (fOUT_335) fprintf(fOUT_335, "date,latitude,longitude,country,epoch exp, epoch obs, availability, oslips, MP12, MP21, MP15, MP51, MP16, MP61, MP17, MP71, MP18, MP81, SN, rcv type\n");
        if (fOUT_IGS) fprintf(fOUT_IGS, "date,latitude,longitude,country,epoch exp, epoch obs, availability, oslips, MP12, MP21, MP15, MP51, MP16, MP61, MP17, MP71, MP18, MP81, SN, rcv type\n");
        for (std::vector<rcv_teqc_t>::iterator pStn = pRcv->second.begin(); pStn != pRcv->second.end(); ++pStn)
        {
            if (pStn->type == 0 && fOUT_IGS) fprintf(fOUT_IGS, "%s,%7.2f,%7.2f,%s,%6i,%6i,%6.2f,%6i,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%s,%s\n", pRcv->first.c_str(), mCoords[pStn->name].lat, mCoords[pStn->name].lon, mCoords[pStn->name].country.c_str(), pStn->total_exp, pStn->total_obs, pStn->rate * 1.0, pStn->oslips, pStn->mp12, pStn->mp21, pStn->mp15, pStn->mp51, pStn->mp16, pStn->mp61, pStn->mp17, pStn->mp71, pStn->mp18, pStn->mp81, pStn->name.c_str(), pStn->rcv_type.c_str());
            if (pStn->type == 1 && fOUT_P40) fprintf(fOUT_P40, "%s,%7.2f,%7.2f,%s,%6i,%6i,%6.2f,%6i,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%s,%s\n", pRcv->first.c_str(), mCoords[pStn->name].lat, mCoords[pStn->name].lon, mCoords[pStn->name].country.c_str(), pStn->total_exp, pStn->total_obs, pStn->rate * 1.0, pStn->oslips, pStn->mp12, pStn->mp21, pStn->mp15, pStn->mp51, pStn->mp16, pStn->mp61, pStn->mp17, pStn->mp71, pStn->mp18, pStn->mp81, pStn->name.c_str(), pStn->rcv_type.c_str());
            if (pStn->type == 2 && fOUT_980) fprintf(fOUT_980, "%s,%7.2f,%7.2f,%s,%6i,%6i,%6.2f,%6i,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%s,%s\n", pRcv->first.c_str(), mCoords[pStn->name].lat, mCoords[pStn->name].lon, mCoords[pStn->name].country.c_str(), pStn->total_exp, pStn->total_obs, pStn->rate * 1.0, pStn->oslips, pStn->mp12, pStn->mp21, pStn->mp15, pStn->mp51, pStn->mp16, pStn->mp61, pStn->mp17, pStn->mp71, pStn->mp18, pStn->mp81, pStn->name.c_str(), pStn->rcv_type.c_str());
            if (pStn->type == 3 && fOUT_335) fprintf(fOUT_335, "%s,%7.2f,%7.2f,%s,%6i,%6i,%6.2f,%6i,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.3f,%s,%s\n", pRcv->first.c_str(), mCoords[pStn->name].lat, mCoords[pStn->name].lon, mCoords[pStn->name].country.c_str(), pStn->total_exp, pStn->total_obs, pStn->rate * 1.0, pStn->oslips, pStn->mp12, pStn->mp21, pStn->mp15, pStn->mp51, pStn->mp16, pStn->mp61, pStn->mp17, pStn->mp71, pStn->mp18, pStn->mp81, pStn->name.c_str(), pStn->rcv_type.c_str());
        }
        if (fOUT_P40) fclose(fOUT_P40);
        if (fOUT_980) fclose(fOUT_980);
        if (fOUT_335) fclose(fOUT_335);
        if (fOUT_IGS) fclose(fOUT_IGS);
    }
    return 0;
}

