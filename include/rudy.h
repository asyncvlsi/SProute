#ifndef RUDY_H
#define RUDY_h

void plot_rudy(float* rudy, Algo algo) {

    cout << xGrid << " " << yGrid << " grid" << endl;
    

    for(int i = 0; i < xGrid * yGrid; i++)
        rudy[i] == 0;


    for(int netID = 0; netID < numValidNets; netID++) {
        int min_x = xGrid + 1;
        int min_y = yGrid + 1;
        int max_x = 0;
        int max_y = 0;

        int deg = nets[netID]->deg;
        for(int pinID = 0; pinID < deg; pinID++) {
            min_x = min(min_x, (int) nets[netID]->pinX[pinID]);
            max_x = max(max_x, (int) nets[netID]->pinX[pinID]);
            min_y = min(min_y, (int) nets[netID]->pinY[pinID]);
            max_y = max(max_y, (int) nets[netID]->pinY[pinID]);
        }

        if(max_x - min_x < 0) {
            cout << "Error: " << min_x << " " << max_x << endl;
            exit(1);
        }

        if(max_y - min_y < 0) {
            cout << "Error: " << min_y << " " << max_y << endl;
            exit(1);
        }

        float wl = max_x - min_x + 1 + max_y - min_y + 1;
        float local_rudy = wl / ((float) (max_x - min_x + 1) * (float) (max_y - min_y + 1));
        if(isnan(local_rudy)) {
            cout << "here: " << min_x << " " << max_x << " " << min_y << " " <<max_y << endl;
            exit(1);
        }
        if(local_rudy <= 0) {
            cout << "Error: negative rudy! " << local_rudy << " " << max_x - min_x << " " << max_y - min_y << endl;
            exit(1);
        }
        for(int x = min_x; x <= max_x; x++) {
            for(int y = min_y; y <= max_y; y++) {
                rudy[y * xGrid + x] += local_rudy;
                if(isnan(rudy[y * xGrid + x])) {
                    cout << min_x << " " << max_x << " " << min_y << " " << max_y << endl;
                    exit(1);
                }
            }
        }
    }

    for(int netID = 0; netID < numInvalidNets; netID++) {
        int x = (int) invalid_nets[netID]->pinX[0];
        int y = (int) invalid_nets[netID]->pinY[0];
        rudy[y * xGrid + x] += 1;
    }

    if(algo == RUDY) {
        float rudy_sum = 0;
        int hist_size = 30;
        int hist[hist_size];
        for(int i = 0; i < hist_size; i++){
            hist[i] = 0;
        }
        
        ofstream outfile("heat.txt");
        cout << "heat? " << endl;
        outfile << "heat" << endl;
        outfile << "(" << endl;
        for(int x = 0; x < xGrid; x++) {
            for(int y = 0; y < yGrid; y++) {
                int llx  = defDB.xGcellBoundaries.at(x);		
                int lly = defDB.yGcellBoundaries.at(y);
                int urx = defDB.xGcellBoundaries.at(x + 1);
                int ury  = defDB.yGcellBoundaries.at(y + 1);
                outfile << llx << " ";
                outfile << lly << " ";
                outfile << urx << " ";
                outfile << ury << " ";
                outfile << rudy[y * xGrid + x] << endl;
                float rudy1 = rudy[y * xGrid + x];
                rudy_sum += rudy1;
                if((int) rudy1 < hist_size)
                    hist[(int) rudy1]++;
                else hist[hist_size - 1]++;
                
            }
        }
        outfile << ")" << endl;
        outfile.close();

        for(int i = 0; i < hist_size; i++) {
            cout  << hist[i] << endl;
        }

        cout << "avg: " << rudy_sum / (float)(xGrid * yGrid) << endl;
    }

}



float plot_pin_density(float* pin_density, Algo algo) {

    cout << xGrid << " " << yGrid << " grid" << endl;
    
    float pin_density_sum = 0;
    for(int i = 0; i < xGrid * yGrid; i++)
        pin_density[i] == 0;


    for(int netID = 0; netID < numValidNets; netID++) {

        int deg = nets[netID]->deg;
        for(int pinID = 0; pinID < deg; pinID++) {
            int x = (int) nets[netID]->pinX[pinID];
            int y = (int) nets[netID]->pinY[pinID];
            if(x != 0 && y != 0 && x != xGrid - 1 && y != yGrid - 1) {
                pin_density[y * xGrid + x] += 1.0;
                pin_density_sum += 1.0;
            }
        }
    }

    for(int netID = 0; netID < numInvalidNets; netID++) {
        int x = (int) invalid_nets[netID]->pinX[0];
        int y = (int) invalid_nets[netID]->pinY[0];
        if(x != 0 && y != 0 && x != xGrid - 1 && y != yGrid - 1) {
            pin_density[y * xGrid + x] += 2.0;
            pin_density_sum += 2.0;
        }
    }

    int* expand_pin_density = new int [xGrid * yGrid];
    for(int x = 0; x < xGrid; x++) {
        for(int y = 0; y < yGrid; y++) {
            expand_pin_density[y * xGrid + x] = 0;
        }
    }

    for(int x = 0; x < xGrid; x++) {
        for(int y = 0; y < yGrid; y++) {
            
            for(int local_x = max(0, x - 2); local_x < min(xGrid - 1, x + 2); local_x++) {
                for(int local_y = max(0, y - 2); local_y < min(yGrid - 1, y + 2); local_y++) {
                    expand_pin_density[y * xGrid + x] += pin_density[local_y * xGrid + local_x];
                }
            }
        }
    }

    if(algo == PIN_DENSITY) {
        
        int hist_size = 100;
        int hist[hist_size];
        for(int i = 0; i < hist_size; i++){
            hist[i] = 0;
        }
        
        ofstream outfile("heat.txt");
        cout << "heat? " << endl;
        outfile << "heat" << endl;
        outfile << "(" << endl;
        for(int x = 0; x < xGrid; x++) {
            for(int y = 0; y < yGrid; y++) {
                int llx  = defDB.xGcellBoundaries.at(x);		
                int lly = defDB.yGcellBoundaries.at(y);
                int urx = defDB.xGcellBoundaries.at(x + 1);
                int ury  = defDB.yGcellBoundaries.at(y + 1);
                outfile << llx << " ";
                outfile << lly << " ";
                outfile << urx << " ";
                outfile << ury << " ";
                expand_pin_density[y * xGrid + x] = (expand_pin_density[y * xGrid + x] < 60)? 0 : expand_pin_density[y * xGrid + x];
                outfile << expand_pin_density[y * xGrid + x]  << endl;
                float pin_density1 = expand_pin_density[y * xGrid + x] ;
                pin_density_sum += pin_density1;
                if((int) pin_density1 < hist_size)
                    hist[(int) pin_density1]++;
                else hist[hist_size - 1]++;

                //if(pin_density1 >= 20)
                //    cout << "high density point > 20 : " << x << " " << y << endl;
                
            }
        }
        outfile << ")" << endl;
        outfile.close();

        for(int i = 0; i < hist_size; i++) {
            cout  << hist[i] << endl;
        }

        cout << "avg: " << pin_density_sum / (float)(xGrid * yGrid) << endl;
    }

    return pin_density_sum / (float)(xGrid * yGrid);

}


void plot_drc_map() {

    ifstream infile("ispd19_test7_metal5.sproute.georpt");

    if(!infile.is_open()) {
        cout << "unable to open rpt file" << endl;
        exit(1);
    }
    
    int* drc_cnt = new int [xGrid * yGrid];

    for(int i = 0; i < xGrid*yGrid; i++)
        drc_cnt[i] = 0;

    int dbuPerMicro = defDB.dbuPerMicro;
    int total_cnt = 0;
    while(!infile.eof()) {
        string tmp;
        infile >> tmp;
        if(tmp == "Metal2") {
            total_cnt++;
            float x1, y1, x2, y2;
            string tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
            infile >> tmp0 >> tmp1;
            infile >> tmp2 >> tmp3 >> x1 >> tmp4 >> y1 >> tmp5 >> tmp6 >> x2 >> tmp7 >> y2 >> tmp8;

            if(tmp0 != ")" || tmp2 != ":" || tmp3 != "(" || tmp4 != "," || tmp5 != ")" || tmp6 != "(" || tmp7 != "," ||
            tmp8 != ")" ) {
                cout << tmp0 << " " << tmp1 << " " << tmp2 <<" " << tmp3 <<" " << tmp4 <<" " << tmp5 <<" " << tmp6;
                cout << " " << tmp7 << " " << tmp8;
                cout << endl;
                cout << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
                exit(1);
            }
            x1 *= dbuPerMicro;
            x2 *= dbuPerMicro;
            y1 *= dbuPerMicro;
            y2 *= dbuPerMicro;

            int x_start = parser::find_Gcell((int) x1, defDB.xGcellBoundaries); //all are inclusive
            int x_end   = parser::find_Gcell((int) x2, defDB.xGcellBoundaries);
            int y_start = parser::find_Gcell((int) y1, defDB.yGcellBoundaries);
            int y_end   = parser::find_Gcell((int) y2, defDB.yGcellBoundaries);

            int x_mid = (x_start + x_end) / 2;
            int y_mid = (y_start + y_end) / 2;
            /*for(int x = x_start; x <= x_end; x++) {
                for(int y = y_start; y <= y_end; y++) {
                    drc_cnt[y * xGrid + x]++;
                }
            }*/
            drc_cnt[y_mid * xGrid + x_mid]++;

        }
    }
    cout << "total number of drcs: " << total_cnt << endl;

    int* expand_drc_cnt = new int [xGrid * yGrid];
    for(int x = 0; x < xGrid; x++) {
        for(int y = 0; y < yGrid; y++) {
            expand_drc_cnt[y * xGrid + x] = 0;
        }
    }
    for(int x = 0; x < xGrid; x++) {
        for(int y = 0; y < yGrid; y++) {
            
            for(int local_x = max(0, x - 2); local_x < min(xGrid - 1, x + 2); local_x++) {
                for(int local_y = max(0, y - 2); local_y < min(yGrid - 1, y + 2); local_y++) {
                    expand_drc_cnt[y * xGrid + x] += drc_cnt[local_y * xGrid + local_x];
                }
            }
        }
    }

    

    int hist_size = 20;
    int hist[hist_size];
    for(int i = 0; i < hist_size; i++){
        hist[i] = 0;
    }
        
    ofstream outfile("heat.txt");
    cout << "heat? " << endl;
    outfile << "heat" << endl;
    outfile << "(" << endl;
    for(int x = 0; x < xGrid; x++) {
        for(int y = 0; y < yGrid; y++) {
            int llx  = defDB.xGcellBoundaries.at(x);		
            int lly = defDB.yGcellBoundaries.at(y);
            int urx = defDB.xGcellBoundaries.at(x + 1);
            int ury  = defDB.yGcellBoundaries.at(y + 1);
            outfile << llx << " ";
            outfile << lly << " ";
            outfile << urx << " ";
            outfile << ury << " ";
            outfile << expand_drc_cnt[y * xGrid + x] << endl;
            int drc_cnt1 = expand_drc_cnt[y * xGrid + x];
            if(drc_cnt1 < hist_size)
                hist[drc_cnt1]++;
            else hist[hist_size - 1]++;
        }
    }
    outfile << ")" << endl;
    outfile.close();
    infile.close();

    for(int i = 0; i < hist_size; i++) {
        cout  << hist[i] << endl;
    }

}

#endif