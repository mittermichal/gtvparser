#include <stdio.h>
#include <curl/curl.h>
#include "curl_help.h"

typedef struct _node 
{
	char*str;
	int time;
	int n;
}
Node;

int prefix(char*s,char*pre)
{
	int i=0;
	while (s[i]&&pre[i]&&s[i]==pre[i])
	{
		i++;
	}
	if (!pre[i]) return 1;
	else return 0;
}

void printnotags(char*str,int n,FILE*out)
{
	char flag=0;
	for (int i=0;i<n;i++)
	{
		if (str[i]=='<') flag=1;
		if (str[i]=='>') flag=0;
		if (!flag) fprintf(out,"%c",str[i]);
	}
}

void gettime(char*str,int*min,int*sec)
{
	char mins[3]={0,0,0};
	char secs[3]={0,0,0};
	mins[0]=str[0];
	if (str[1]==':')
	{
		secs[0]=str[2];
		secs[1]=str[3];
	}
	else
	{
		mins[1]=str[1];
		secs[0]=str[3];
		secs[1]=str[4];
	}
	*min=atoi(mins);
	*sec=atoi(secs);
}

int main(void)
{
	/*
	//TESTS
	fprintf(out,"%d\n",strlen("<td class=\"left playername\">"));
	fprintf(out,"%d %d %d %d %d\n",prefix("abcde","abc"),prefix("abcde","bc"),
		prefix("abcde","abcdef"),prefix("abcde","abcde"),prefix("ab","abc"));
	*/
int year_in;
printf("Enter year: ");scanf("%d",&year_in);

for (int year=year_in;year<2014;year++) 
{
for (int month=1;month<13;month++)
{
	printf("%d %d\n",month,year);
	char smonth[3];
	itoa(month,smonth,10);
	char syear[5];
	itoa(year,syear,10);
	char filename[20];
	strcpy(filename,"out/");
	strcpy(filename+strlen(filename),syear);
	strcpy(filename+strlen(filename),"-");
	strcpy(filename+strlen(filename),smonth);
	strcpy(filename+strlen(filename),".html");
	FILE*out=fopen(filename,"w");
	if (!out) return -1;
	  CURL *curl;
	  CURLcode res;
	  curl = curl_easy_init();
	  if(curl) {
		struct string ss;
		init_string(&ss);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ss);
	  //char url[]="http://www.gamestv.org/event/33955-dead-ringers-vs-to-make-odds-even/statistics";
		// get links from calendar
		char list[50];
		
		strcpy(list,"http://www.gamestv.org/schedule/list/");
		strcpy(list+strlen(list),smonth);
		strcpy(list+strlen(list),"/");
		strcpy(list+strlen(list),syear);
		curl_easy_setopt(curl, CURLOPT_URL, list);
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
		{  
			/*printf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));*/
			return 1;
		}
		char*replay=strstr(ss.ptr,"alt=\"replay\"");
		int url_count=0;
		char**url;
	  url=(char**)malloc(800*sizeof(char*));
		while (replay)
		{
			int i=0;
			while (!prefix(replay+i,"<a href=\"/event/")) {i++;}
			//url[url_count]=
			int j;
			for (j=0;replay[i+j+10]!='\"';j++) {}
			url[url_count]=(char*)malloc((j+35)*sizeof(char));
			strcpy(url[url_count],"http://www.gamestv.org/");
			strncpy(url[url_count]+23,replay+i+10,j);
			strcpy(url[url_count]+23+j,"statistics");
			url[url_count][23+j+10]=0;
			url_count++;
			replay=strstr(replay+1,"alt=\"replay\"");
		}

	  //url[0]="http://www.gamestv.org/event/2112-team-dignitas-vs-zeropoint-gaming/statistics/";
  
		//res = curl_easy_perform(curl);
	 for (int url_i=0;url_i<url_count;url_i++)
	 {
			curl = curl_easy_init();
				if(!curl)	return 1;

			init_string(&ss);
			curl_easy_setopt(curl, CURLOPT_URL, url[url_i]);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ss);
			/* Perform the request, res will get the return code */ 
			res = curl_easy_perform(curl);
			/* Check for errors */ 
			if(res != CURLE_OK)
			{  
				/*printf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));*/
				return 1;
			}
	

			//parsing
			printf("parsing...%d %d [%d] %s\n",month,year,url_i,url[url_i]);

			fprintf(out,"<div style=\"border: 2px solid #FFD292;border-radius: 10px;-moz-border-radius: 10px;\">\n");
			fprintf(out,"<a style=\"color:black\" href=\"%s\">%s</a></br>\n",url[url_i],url[url_i]);
		
			char*endstats=strstr(ss.ptr,"View plaintext endround stats");
			if (endstats) *endstats=0;
			else 
			{
				fprintf(out,"</div>\n");
				continue;
				free(ss.ptr);
				curl_easy_cleanup(curl);
			}

			int spree=3;
			int diff=4;


			int count=0;
			//fprintf(out,"%s\n", strstr(s,"<div class=\"matchStats\""));
			char*mapstats=strstr(ss.ptr,"</legend><table rules=\"groups\""); //preskocenie entire match
			if (mapstats) mapstats=strstr(mapstats+1,"</legend><table rules=\"groups\"");
			while (mapstats) //for each map
			{
				//print map name
				int i=0;
				while (!prefix(mapstats-i,"for ")) {i++;} 
				fprintf(out,"<p style=\"margin:0 0 0 15 \">");for (int j=0;j<i-4;j++) fprintf(out,"%c",mapstats[-i+j+4]);fprintf(out,"</p>\n");

					char*rounds=strstr(mapstats," <span class=\"grey\">(click columns to sort, the [+] to expand)");
					int rounds_count=0;
					while (rounds&&rounds_count<2) // for each round
					{
						fprintf(out,"<p style=\"margin:0 0 0 30 \">");for (int j=0;j<9;j++) fprintf(out,"%c",rounds[-9+j]);fprintf(out,"</p>\n"); //print # of round
						char*player=strstr(rounds,"<td class=\"left playername\">");
						if (!player) continue;
						for (int a=0;a<2;a++) 
						{
							if (a==0) player=strstr(rounds,"<td class=\"left playername\">");
							else {
								player=strstr(player,"<td class=\"left playername\">");
								player=strstr(player+1,"<td class=\"left playername\">");
							}
							char*players_end=strstr(player,"<tbody class=\"sum\">");
							//players_end=strstr(players_end+1,"</tbody>");

							//find end of players list
							/*int i=0;
							while (!prefix(players_end-i,"<td class=\"left playername\">")) {i++;}
							players_end-=i;*/

							while(player&&player<players_end)  // for each player
							{
								// print player name GTV BUGGED
								//int i=0;
								//while (!prefix(player+i,"</td>")) {i++;} 
								//fprintf(out,"   ");for (int j=0;j<i;j++) fprintf(out,"%c",player[j]);fprintf(out,"\n");
								//fprintf(out,"    ");printnotags(player,i,out);fprintf(out,"\n");
					
								char*kill=strstr(player," class=\"k\">K</span>");
								char*kills_end;
									if (kill) kills_end=strstr(kill,"</td>");

								int first_kill=1;
								int top=0;
								while (kill&&kill<kills_end) // for each kill
								{

						
									//print title of kill
									int k=0;
									while (!prefix(kill-k,"<span title=")) {k++;} 
									//fprintf(out,"       ");for (int j=0;j<i-14;j++) fprintf(out,"%c",kill[13+j-i]);fprintf(out,"\n");
						
									//get time
									int min=0;
									int sec=0;
									i=0;
									while (!prefix(kill-i,"(")) {i++;}
									gettime(kill-i+1,&min,&sec);

									//MULTIKILL STACK
						
									Node stack[1000];
									Node n;
									n.str=kill+13-k;
									n.time=60*min+sec;
									n.n=k-14;
									if (first_kill) //add first kill to stack
									{
										stack[top]=n;
										top++;
										first_kill=0;
										//fprintf(out,"       ");for (int j=0;j<stack[0].n;j++) fprintf(out,"%c",stack[0].str[j]);fprintf(out,"\n");
									}
									else if (stack[top-1].time-n.time<=diff) //add to stack
									{
										stack[top]=n;
										top++;
										first_kill=0;
									}
									else if (top>spree-1)
									{
										fprintf(out,"<p style=\"margin:0 0 0 45 \">Spree: %d Time: %d secs</p>\n",top,stack[0].time-stack[top-1].time);
										for (int i=0;i<top;i++) 
											{
											
												fprintf(out,"<p style=\"margin:0 0 0 60 \">");for (int j=0;j<stack[i].n;j++) fprintf(out,"%c",stack[i].str[j]);fprintf(out,"</p>\n");
						
											}
										top=0;
										stack[top]=n;
										top++;
										first_kill=0;
									}
									else
									{
										top=0;
										stack[top]=n;
										top++;
										first_kill=0;
									}
									//fprintf(out,"          %d : %d\n",min,sec);
									count++;
									kill=strstr(kill+1," class=\"k\">K</span>");
									if ((!(kill&&kill<kills_end))&&(top>spree-1)) //end of kills of players and some spree left on stack
									{
										fprintf(out,"<p style=\"margin:0 0 0 45 \">Spree: %d Time: %d secs</p>\n",top,stack[0].time-stack[top-1].time);
										for (int i=0;i<top;i++) 
										{
											fprintf(out,"<p style=\"margin:0 0 0 60 \">");for (int j=0;j<stack[i].n;j++) fprintf(out,"%c",stack[i].str[j]);fprintf(out,"</p>\n");
						
										}
									}
								}
					
								player=strstr(player+1,"<td class=\"left playername\">");
							}
						}

						rounds=strstr(rounds+1," <span class=\"grey\">(click columns to sort, the [+] to expand)");
						rounds_count++;
					}
				mapstats=strstr(mapstats+1,"</legend><table rules=\"groups\"");;
			}
			//fprintf(out,"%d\n",count);
			fprintf(out,"</div>\n",count);
		free(ss.ptr);
		curl_easy_cleanup(curl);
		}
		/* always cleanup */ 
		fclose(out);
	  }
  }
  }
  return 0;
}