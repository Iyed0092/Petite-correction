#include <stdio.h>
#include <stdlib.h>
#include <string.h>

    int i=0;
    int deb=0;
    double v;
    double tauxDecouvert=0.09;
    double tauxmvtdebit=0.00025;
    double tauxCPFD=0.0005;
    int a=0;
    double agios=0;
    
    char choix=' ';
    struct date {
    int jj;
    int mm;};
    char dateEntree[6];


struct tableRow{
    char dateOp[100];
    char dateVal[100];
    double mvtDeb;
    double mvtCred;
    double soldeDeb;
    double soldeCred;
    int nbJours;
    double nbDebit;
    double nbCred;
    double tauxInt;
    double ID;
    double CPFD;
    double CD;
};

struct tableRow table[100];
void ajoutSomme(struct tableRow *table, int i, double v)
{   
    if (v > 0) {
        (table + i)->mvtCred = v;
    } else {
        (table + i)->mvtDeb = -v;  
    }

    if ((table+i-1)->soldeDeb>0)
    {
        if (v<0)
        {
            (table+i)->soldeDeb=(table+i-1)->soldeDeb-v;
        }
        if (v>0)
        {
            if (v>(table+i-1)->soldeDeb)
            {
                (table+i)->soldeCred=v-(table+i-1)->soldeDeb;
            }
            else
            {
                (table+i)->soldeDeb=(table+i-1)->soldeDeb-v;
            }
        }
    }

   if ((table+i-1)->soldeCred>0)
   {
        if (v>0)
        {
            (table+i)->soldeCred=(table+i-1)->soldeCred+v;
        }
        else
        {
            if (-v>(table+i-1)->soldeCred)
            {
                (table+i)->soldeDeb=(table+i-1)->soldeCred+v;
            }
            else
            {
                (table+i)->soldeCred= (table+i-1)->soldeCred+v;
            }
        }
   }






}



struct date convertirDate(char ch[100]) {
    int i = 0;
    struct date date;
    char ch1[3], ch2[3]; 

    ch1[0]=ch[0];
    ch1[1]=ch[1];
    ch2[0] = ch[3];
    ch2[1]=ch[4];
    date.jj=atoi(ch1);
    date.mm=atoi(ch2);
    return date;
}

int memois(struct tableRow *table,int i)
{
    struct date date1=convertirDate((table+i-1)->dateVal);
    struct date date2=convertirDate((table+i)->dateVal);
    int m1=date1.mm;
    int m2=date2.mm;
    return m1==m2;
}

int diffdDates(struct tableRow *table,int i)
{
    struct date date1=convertirDate((table+i-1)->dateVal);
    struct date date2=convertirDate((table+i)->dateVal);
    int m1=date1.mm;
    int m2=date2.mm;
    int j1=date1.jj;
    int j2=date2.jj;
    int result=(m2*30+j2)-(m1*30+j1);
    return result;

}

void calcCPFD(int *a, struct tableRow *table, int i)
{   int max=*a;
    for (int j=*a;j<i;j++)
    {
        if ((table+j)->soldeDeb>(table+max)->soldeDeb)
        {
            max=j;
        }
        
    }
    (table+max)->CPFD=(table+max)->soldeDeb*tauxCPFD;
    *a=(i+1);
}

int finTrimestre(struct tableRow t) {
    return (strcmp(t.dateVal, "30-09") == 0 || strcmp(t.dateVal, "31-03") == 0 || strcmp(t.dateVal, "31-12") == 0 || strcmp(t.dateVal, "30-06") == 0);
}

double calcAgios(struct tableRow *table,int i,int deb)
{
    double result=0;
    for (int j=deb;j<i;j++)
    {
        result+=(table+j)->ID + (table+j)->CPFD + (table+j)->CD;
    }
    return result;
}

void completerTrimestre(struct tableRow *table,int i,int *a,double *agios)
{
    struct date date1=convertirDate((table+i-1)->dateVal);
    if (date1.mm<=3)
    {
       strcpy((table+i)->dateVal, "31-03");
    }
    else if (date1.mm<=6 && date1.mm>3)
    {
       strcpy((table+i)->dateVal, "30-06");
    }
    else if (date1.mm<=9 && date1.mm>6)
    {
        strcpy((table+i)->dateVal, "30-09");
    }
    else 
    {
        strcpy((table+i)->dateVal, "31-12");
    }
    int j = diffdDates(table, i);
    (table+i-1)->nbJours = j;
    (table+i-1)->nbCred = j * (table+i-1)->soldeCred;
    (table+i-1)->nbDebit = j * (table+i-1)->soldeDeb;
    (table+i-1)->ID = (table+i-1)->nbDebit * tauxDecouvert / 360;
    (table+i-1)->CD = (table+i-1)->mvtDeb * tauxmvtdebit;
    
    (table+i)->mvtDeb = 0;
    (table+i)->mvtCred = 0;
    (table+i)->soldeCred = (table+i-1)->soldeCred;
    (table+i)->soldeDeb = (table+i-1)->soldeDeb;
    (table+i)->nbJours = 0;
    (table+i)->nbCred = 0;
    (table+i)->nbDebit = 0;
    (table+i)->tauxInt = tauxDecouvert;
    (table+i)->ID = 0;
    (table+i)->CPFD = 0;
    (table+i)->CD = 0;
    
    calcCPFD(a, table, i );
    *agios = calcAgios(table, i,deb);
    
}
    int comparerDate(char date11[6], char date22[6])
    {   struct date date1;
        struct date date2;
        date1=convertirDate(date11);
        date2=convertirDate(date22);
        if (date1.mm<date2.mm)
        {
            return 2;
        }
        else if (date1.mm>date2.mm)
        {
            return 1;
        }
        else
        {
            if (date1.jj<date2.jj)
            {
                return 2;
            }
            else if(date1.jj>date2.jj)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }


    void ajouterlaligne(struct tableRow *table, int *i, char dateEntree[6],int *a,double *agios,int *deb)
    {
        if ( comparerDate(dateEntree,"31-03")==1 &&  comparerDate((table+*i-1)->dateVal,"31-03")==2)
        {   
            strcpy((table + *i)->dateVal, "31-03");
            int j = diffdDates(table, *i);
            (table+*i-1)->nbJours = j;
            (table+*i-1)->nbCred = j * (table+*i-1)->soldeCred;
            (table+*i-1)->nbDebit = j * (table+*i-1)->soldeDeb;
            (table+*i-1)->ID = (table+*i-1)->nbDebit * tauxDecouvert / 360;
            (table+*i-1)->CD = (table+*i-1)->mvtDeb * tauxmvtdebit;
            (table+*i)->mvtDeb = 0;
            (table+*i)->mvtCred = 0;
            (table+*i)->soldeCred = (table+*i-1)->soldeCred;
            (table+*i)->soldeDeb = (table+*i-1)->soldeDeb;
            (table+*i)->nbJours = 0;
            (table+*i)->nbCred = 0;
            (table+*i)->nbDebit = 0;
            (table+*i)->tauxInt = tauxDecouvert;
            (table+*i)->ID = 0;
            (table+*i)->CPFD = 0;
            (table+*i)->CD = 0;
            calcCPFD(a, table, *i );
            *agios = calcAgios(table, *i,*deb );
            *deb=*i+1;
    
            if ((table+*i-1)->soldeDeb>0)
                {
                    (table+*i)->soldeDeb+=*agios;
                }
              
            
            else 
            {
                if ((table+*i-1)->soldeCred<*agios)
                {
                     (table+*i)->soldeDeb=*agios-(table+*i-1)->soldeCred;
                }
                else
                {
                    (table+*i-1)->soldeCred-=*agios;
                }
            }
            (*i)++;
        }
        
         if ( comparerDate(dateEntree,"30-06")==1 &&  comparerDate((table+*i-1)->dateVal,"30-06")==2)
        {   
            strcpy((table + *i)->dateVal, "30-06");
            int j = diffdDates(table, *i);
            (table+*i-1)->nbJours = j;
            (table+*i-1)->nbCred = j * (table+*i-1)->soldeCred;
            (table+*i-1)->nbDebit = j * (table+*i-1)->soldeDeb;
            (table+*i-1)->ID = (table+*i-1)->nbDebit * tauxDecouvert / 360;
            (table+*i-1)->CD = (table+*i-1)->mvtDeb * tauxmvtdebit;
            (table+*i)->mvtDeb = 0;
            (table+*i)->mvtCred = 0;
            (table+*i)->soldeCred = (table+*i-1)->soldeCred;
            (table+*i)->soldeDeb = (table+*i-1)->soldeDeb;
            (table+*i)->nbJours = 0;
            (table+*i)->nbCred = 0;
            (table+*i)->nbDebit = 0;
            (table+*i)->tauxInt = tauxDecouvert;
            (table+*i)->ID = 0;
            (table+*i)->CPFD = 0;
            (table+*i)->CD = 0;
            calcCPFD(a, table, *i );
           if ((table+*i-1)->soldeDeb>0)
                {
                    (table+*i)->soldeDeb+=*agios;
                }
              
            
            else  
            {
                if ((table+*i-1)->soldeCred<*agios)
                {
                     (table+*i)->soldeDeb=*agios-(table+*i-1)->soldeCred;
                }
                else
                {
                    (table+*i-1)->soldeCred-=*agios;
                }
            }
            *deb=*i+1;
             ajoutSomme(table, *i, *agios);
            (*i)++;
       
        }
        
        if ( comparerDate(dateEntree,"30-09")==1 &&  comparerDate((table+*i-1)->dateVal,"30-09")==2)
        {   
            strcpy((table + *i)->dateVal, "30-09");
            int j = diffdDates(table, *i);
            (table+*i-1)->nbJours = j;
            (table+*i-1)->nbCred = j * (table+*i-1)->soldeCred;
            (table+*i-1)->nbDebit = j * (table+*i-1)->soldeDeb;
            (table+*i-1)->ID = (table+*i-1)->nbDebit * tauxDecouvert / 360;
            (table+*i-1)->CD = (table+*i-1)->mvtDeb * tauxmvtdebit;
            (table+*i)->mvtDeb = 0;
            (table+*i)->mvtCred = 0;
            (table+*i)->soldeCred = (table+*i-1)->soldeCred;
            (table+*i)->soldeDeb = (table+*i-1)->soldeDeb;
            (table+*i)->nbJours = 0;
            (table+*i)->nbCred = 0;
            (table+*i)->nbDebit = 0;
            (table+*i)->tauxInt = tauxDecouvert;
            (table+*i)->ID = 0;
            (table+*i)->CPFD = 0;
            (table+*i)->CD = 0;
            calcCPFD(a, table, *i );
           if ((table+*i-1)->soldeDeb>0)
                {
                    (table+*i)->soldeDeb+=*agios;
                }
              
            
            else  
            {
                if ((table+*i-1)->soldeCred<*agios)
                {
                     (table+*i)->soldeDeb=*agios-(table+*i-1)->soldeCred;
                }
                else
                {
                    (table+*i-1)->soldeCred-=*agios;
                }
            }
            *deb=*i+1;
            (*i)++;
           
        }
        

        
        
    }


void saveTableToFile(struct tableRow *table, int size, const char *filename) {
    FILE *f = fopen(filename, "at");

    if (f == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    for (int i = 0; i < size; ++i) {
        fprintf(f, "%s %s %.2f %.2f %.2f %.2f %d %.2f %.2f %.2f %.2f %.2f %.2f\n",
                (table + i)->dateOp, (table + i)->dateVal, (table + i)->mvtDeb, (table + i)->mvtCred,
                (table + i)->soldeDeb, (table + i)->soldeCred, (table + i)->nbJours, (table + i)->nbDebit,
                (table + i)->nbCred, (table + i)->tauxInt, (table + i)->ID, (table + i)->CPFD, (table + i)->CD);
    }

    fclose(f);
    printf("Table saved to file successfully.\n");
}

int main() {

    do {
        printf("donner date valeur \n");
        scanf("%s",&dateEntree);
        printf("donner l'opération à effectuer \n");
        scanf("%lf", &v);
        
        if (i == 0) {
            strcpy((table + i)->dateVal, dateEntree);
            if (v < 0) {
                table[i].mvtDeb = -v;
                table[i].mvtCred = 0;
                table[i].soldeCred = 0;
                table[i].soldeDeb = -v;
                table[i].nbJours = 0;
                table[i].nbCred = 0;
                table[i].nbDebit = 0;
                table[i].tauxInt = tauxDecouvert;
                table[i].ID = 0;
                table[i].CPFD = 0;
                table[i].CD = 0;
            } else {
                table[i].mvtDeb = 0;
                table[i].mvtCred = v;
                table[i].soldeCred = v;
                table[i].soldeDeb = 0;
                table[i].nbJours = 0;
                table[i].nbCred = 0;
                table[i].nbDebit = 0;
                table[i].tauxInt = tauxDecouvert;
                table[i].ID = 0;
                table[i].CPFD = 0;
                table[i].CD = 0;
            }
            } 

        else {  
                ajouterlaligne(table,&i,dateEntree,&a,&agios,&deb);
                strcpy((table + i)->dateVal, dateEntree);
                ajoutSomme(table, i, v);
                table[i].nbJours = 0;
                table[i].nbDebit = 0;
                table[i].tauxInt = tauxDecouvert;
                table[i].ID = 0;
                table[i].CPFD = 0;
                table[i].CD = 0;
                int j = diffdDates(table, i);
                table[i-1].nbJours = j;
                table[i-1].nbCred = j * table[i-1].soldeCred;
                table[i-1].nbDebit = j * table[i-1].soldeDeb;
                table[i-1].ID = table[i-1].nbDebit * tauxDecouvert / 360;
                table[i-1].CD = table[i-1].mvtDeb * tauxmvtdebit;

                if (!memois(table, i)) {
                    calcCPFD(&a, table, i );
                 }

                }
            
             i++;
            printf("Taper 'q' pour quitter !\n");
            scanf(" %c", &choix);

    } while (choix != 'q');
    
    completerTrimestre(table,i,&a,&agios);
       if ((table+i-1)->soldeDeb>0)
                    {
                        (table+i)->soldeDeb+=agios;
                    }
                  
                
                else  
                {
                    if ((table+i-1)->soldeCred<agios)
                    {
                         (table+i)->soldeDeb=agios-(table+i-1)->soldeCred;
                    }
                    else
                    {
                        (table+i-1)->soldeCred-=agios;
                    }
                }

    
    printf("%-10s %-10s %-10s %-12s %-12s %-8s %-14s %-12s %-10s %-12s %-12s %-12s\n",
        "DateVal", "MVTDEB", "MVTCRED", "SOLDEDEB", "SOLDECRED", "NBJOURS", "NBREDEB", "NBRECRED", "TAUXINT", "ID", "CPFD", "CD");
    

    for (int k = 0; k < i + 1; k++) {
        printf("%-10s %-10.2f %-10.2f %-12.2f %-12.2f %-8d %-14.2f %-12.2f %-10.2f %-12.2f %-12.2f %-12.2f\n",
            table[k].dateVal, table[k].mvtDeb, table[k].mvtCred, table[k].soldeDeb, table[k].soldeCred,
            table[k].nbJours, table[k].nbDebit, table[k].nbCred, table[k].tauxInt, table[k].ID, table[k].CPFD, table[k].CD);
    
        printf("\n");
    }









        FILE *file = fopen("comptecourant.txt", "at");

    
    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

   
    fprintf(file, "%-10s %-10s %-10s %-12s %-12s %-8s %-14s %-12s %-10s %-12s %-12s %-12s\n",
        "DateVal", "MVTDEB", "MVTCRED", "SOLDEDEB", "SOLDECRED", "NBJOURS", "NBREDEB", "NBRECRED", "TAUXINT", "ID", "CPFD", "CD");

    
    for (int k = 0; k < i + 1; k++) {
        fprintf(file, "%-10s %-10.2f %-10.2f %-12.2f %-12.2f %-8d %-14.2f %-12.2f %-10.2f %-12.2f %-12.2f %-12.2f\n",
            table[k].dateVal, table[k].mvtDeb, table[k].mvtCred, table[k].soldeDeb, table[k].soldeCred,
            table[k].nbJours, table[k].nbDebit, table[k].nbCred, table[k].tauxInt, table[k].ID, table[k].CPFD, table[k].CD);
    }

    fclose(file);


    
    return 0;}