/* $Id: entry_ex.c,v 1.17 2016/12/04 15:22:16 tom Exp $ */

#define HAVE_NCURSESW_NCURSES_H
// #include <cdk_test.h>
#include <locale.h>
// GSchade 25.9.18
// #include "entry_ex.h"
#include "efdr.h"
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
SScreen *allgscr;

#ifdef HAVE_XCURSES
char *XCursesProgramName = "entry_ex";
#endif
#include <vector>
#include <string>
#include <cerrno>   // für errno, perror
#include <iostream> // für cout
#include <algorithm> // für sort
using namespace std;
vector<string> erg;

vector<string> userList,myUserList;
static int userSize;


vector<string> myUndopList;
struct UNDO
{
	int deleted;			/* index in current list which is deleted */
	int original;		/* index in myUserList[] of deleted item */
	int position;		/* position before delete */
	int topline;			/* top-line before delete */
};
//static UNDO *myUndoList;
static int undoSize;

// static BINDFN_PROTO(XXXCB);
static int XXXCB(EObjectType cdktype GCC_UNUSED,
		void *object GCC_UNUSED,
		void *clientData GCC_UNUSED,
		chtype key GCC_UNUSED)
{
	vector<string> mesg(1);
	mesg[0]="Hilfefunktion";
	((SScreen*)allgscr)->popupLabel(/*(SScreen*)allgscr,*/
		mesg
		);
//	printf("Hilfefunktion aufgerufen\n\r\n\r");
	return(TRUE);
}

/*
 * This reads the passwd file and retrieves user information.
 */
static int getUserList(vector<string> *plistp)
{
	int x = 0;
#if defined(HAVE_PWD_H)
	struct passwd *ent;
	errno=0;
	endpwent();
	while ((ent = getpwent())) {
		plistp->push_back(ent->pw_name);
	}
	if (errno) {
		perror("Fehler beim Benutzerholen");
	}
	endpwent();
	sort(plistp->begin(),plistp->end());
	return plistp->size();
#endif
	return x;
}

//#define brauchtsaano
enum eingtyp {
	eingfld,
	auswfld,
	dteifld,
};
struct hotkst {
	const char *label;
	int highnr;
	eingtyp obalph;
	int highinr; // fuer jeden vorausgehenden Umlaut usw. 2 Buchstaben rechnen
	int buch;
	CDKOBJS *eingabef;
} hk[]={
	//		 /*
	{"</R/U/6>Directory:<!R!6!U> ",4,auswfld},
	//		 */
	// /*
	{"</R/U/6>Däteis:<!R!6!U> ",2,auswfld},
	{"</R/U/6>Datei:<!R!6>",3,auswfld},
	{"</R/U/6>Döüßatei:<!R!6>",4,auswfld},
	{"</R/U/6>Ordner:<!R!6>",4,auswfld},
	{"</R/U/6>Alphälißt:<!R!6>",4},
	{"</R/U/6>Betalist:<!R!6>",2,dteifld},
	{"</R/U/6>Betalist:<!R!6>",3},
	{"</R/U/6>Dürectory:<!R!6>",3},
	{"</R/U/4>Dürectory:<!R!4>",3},
	{"</R/U/6>Directory:<!R!6!U> ",4,auswfld},
	{"</R/U/6>Däteis:<!R!6!U> ",2,auswfld},
	{"</R/U/6>Datei:<!R!6>",3,auswfld},
	{"</R/U/6>Döüßatei:<!R!6>",4,auswfld},
	{"</R/U/6>Ordner:<!R!6>",4,auswfld},
	{"</R/U/6>Alphälißt:<!R!6>",4},
	{"</R/U/6>Betalist:<!R!6>",2},
	{"</R/U/6>Betalist:<!R!6>",3},
	{"</R/U/6>Dürectory:<!R!6>",3},
	{"</R/U/4>Dürectory:<!R!4>",3},
	// */
};
const int maxhk=sizeof hk/sizeof *hk;
const int yabst=7;
const int xpos=11;

/*
 * This demonstrates the Cdk entry field widget.
 */
int main(int argc, char **argv)
{
	//setlocale(LC_ALL,"de_DE.UTF-8");
	setlocale(LC_ALL,"");
	/* *INDENT-EQLS* */
	SScreen *cdkscreen = 0;

	/* Get the user list. */
	userSize = getUserList(&userList);
	if (userSize <= 0) {
		fprintf(stderr, "Cannot get user list\n");
		ExitProgram(EXIT_FAILURE);
	}
	myUserList = userList;
	undoSize = 0;
  SScreen cscr(0);
	allgscr=cdkscreen=&cscr;
	/*static*/ int maxy=getmaxy(cdkscreen->window)-yabst;
	/*static*/ int maxh=maxy>maxhk?maxhk:maxy;
	/*static*/ int ymin=0;
	/*static*/ int ymax=maxh;
	/*static*/ bool erstmals=1;
	// static size_t maxy=getmaxy(cdkscreen->window)-yabst;
	// static size_t maxh=maxy>maxhk?maxhk:maxy;
	/* Start CDK colors. */
	initCDKColor();
	const int maxlen=100;
	for(size_t aktent=0;aktent<maxhk;aktent++) {
		bool nichtzaehl=0;
		hk[aktent].highinr=0;
		int hnr=1;
//		for(int i=0;i<strlen(hk[aktent].label);i++) {
//			mvwprintw(cdkscreen->window,20+i,aktent*10,"%c(%i)",(unsigned char)hk[aktent].label[i],(unsigned char)hk[aktent].label[i]);
//		}
		for(size_t i=0;i<strlen(hk[aktent].label);i++) {
			if (hk[aktent].label[i]=='>') nichtzaehl=0;
			else if (nichtzaehl) {}
			else if (hk[aktent].label[i]=='<') nichtzaehl=1;
			else {
				hk[aktent].highinr++;
				if (hnr==hk[aktent].highnr) {
					if ((unsigned char)hk[aktent].label[i]==194 ||(unsigned char)hk[aktent].label[i]==195) {
						hk[aktent].buch=(unsigned char)hk[aktent].label[i]*256+(unsigned char)hk[aktent].label[i+1];
					} else
						hk[aktent].buch=(unsigned char)hk[aktent].label[i];
					break;
				}
				else if ((unsigned char)hk[aktent].label[i]==194 ||(unsigned char)hk[aktent].label[i]==195) {}
				else hnr++;
			}
		}
//		mvwprintw(cdkscreen->window,18,aktent*10,"%i/%i",hk[aktent].highnr,hk[aktent].highinr);
//		if (hk[aktent].highnr)
//			mvwprintw(cdkscreen->window,aktent+yabst,125,"%i:%c,%s",hk[aktent].highnr,hk[aktent].buch,hk[aktent].label);
		switch (hk[aktent].obalph) {
			case auswfld:
				hk[aktent].eingabef=
					//newCDKAlphalist(cdkscreen,xpos,yabst+aktent,10,40,"",hk[aktent].label,(CDK_CSTRING*)userList,userSize,'.',A_REVERSE,0,0,hk[aktent].highinr);
					new SAlphalist(cdkscreen,xpos,yabst+aktent,10,40,"",hk[aktent].label, &userList, '.',A_REVERSE,0,0,hk[aktent].highinr);
				break;
			case eingfld:
				hk[aktent].eingabef=
					//newCDKEntry(cdkscreen,xpos,yabst+aktent,"",hk[aktent].label,A_NORMAL,'.',vMIXED,30,0,maxlen,0,0,hk[aktent].highinr);
					new SEntry(cdkscreen,xpos,yabst+aktent,"",hk[aktent].label,A_NORMAL,'.',vMIXED,30,0,maxlen,0,0,hk[aktent].highinr);
				hk[aktent].eingabef->bindCDKObject(/*vENTRY, hk[aktent].eingabef, */'?', XXXCB, 0);
				break;
			case dteifld:
				hk[aktent].eingabef=
					//newCDKEntry(cdkscreen,xpos,yabst+aktent,"",hk[aktent].label,A_NORMAL,'.',vMIXED,30,0,maxlen,0,0,hk[aktent].highinr);
					new SFSelect(cdkscreen,xpos,yabst+aktent,20,65,"",hk[aktent].label,A_NORMAL,'.',A_REVERSE,"</5>","</48>","</N>","</N>",TRUE,FALSE,hk[aktent].highinr);
				hk[aktent].eingabef->bindCDKObject(/*vENTRY, hk[aktent].eingabef, */'?', XXXCB, 0);
				break;

		}
		/* Is the widget null? */
		if (!hk[aktent].eingabef) {
			/* Clean up. */
			cdkscreen->destroyCDKScreen();
			endCDK();
			printf ("Cannot create the entry box. Is the window too small, aktent: %lu   \n",aktent);
			ExitProgram(EXIT_FAILURE);
		}
	}

	/*
	 * Pass in whatever was given off of the command line. Notice we
	 * don't check if argv[1] is null or not. The function setCDKEntry
	 * already performs any needed checks.
	 */
	//setCDKEntry(hk[0].eingabef, argv[optind], 0, max, TRUE);

	/* Activate the entry field. */
	int Znr=0,Zweitzeichen=0,Drittzeichen=0;
	while (1) {
		akteinbart=einb_direkt;
		/* Draw the screen. */
		// refreshCDKScreen(cdkscreen);
		//zeichne(cdkscreen,Znr);

		bool obverschiebe=erstmals;
		int korr;
		if (Znr<ymin) {
			korr=ymin-Znr;
			ymin-=korr;
			ymax-=korr;
			obverschiebe=1;
		} else if (Znr>=ymax) {
			korr=Znr-ymax+1;
			ymin+=korr;
			ymax+=korr;
			obverschiebe=1;
		} 
		if (obverschiebe) {
			//			mvwprintw(cdkscreen->window,1,xpos,"mit Neuzeichnen: %i-%i, Znr: %i  ",ymin,ymax,Znr);
			for(int aktent=0;aktent<maxhk;aktent++) {
				if (aktent>=ymin && aktent<ymax) {
					hk[aktent].eingabef->isVisible=1;
					switch (hk[aktent].obalph) {
						case auswfld:
							((SAlphalist*)hk[aktent].eingabef)->moveCDKAlphalist(/*((SAlphalist*)hk[aktent].eingabef),*/xpos,yabst+aktent-ymin,0,1);
							break;
						case eingfld:
							((SEntry*)hk[aktent].eingabef)->moveCDKEntry(/*hk[aktent].eingabef,*/xpos,yabst+aktent-ymin,0,1);
							break;
						case dteifld:
							((SFSelect*)hk[aktent].eingabef)->moveCDKFselect(/*hk[aktent].eingabef,*/xpos,yabst+aktent-ymin,0,1);
							break;
					}
				} else {
					hk[aktent].eingabef->isVisible=0;
				}
			}
		} else {
			//			mvwprintw(cdkscreen->window,1,xpos,"ohne Neuzeichnen: %i-%i, Znr: %i  ",ymin,ymax,Znr);
		}
		refreshCDKWindow(cdkscreen->window);
		cdkscreen->refreshCDKScreen();
		erstmals=0;
		//char *info;
		// mvwprintw(cdkscreen->window,30,60,"<R>werde eingegeben:%i %i ",info,Zweitzeichen);
					switch (hk[Znr].obalph) {
						case auswfld:
			akteinbart=einb_alphalist;
			/*info=*/((SAlphalist*)hk[Znr].eingabef)->activateCDKAlphalist(/*(SAlphalist*)hk[Znr].eingabef, */0,&Zweitzeichen, &Drittzeichen,/*obpfeil*/0);
			(((SAlphalist*)hk[Znr].eingabef)->scrollField)->eraseCDKScroll(/*((SAlphalist*)hk[Znr].eingabef)->scrollField*/);
							break;
						case eingfld:
			/*info = */((SEntry*)hk[Znr].eingabef)->activateCDKEntry(/*hk[Znr].eingabef, */0,&Zweitzeichen, &Drittzeichen,/*obpfeil*/1);
							break;
						case dteifld:
			/*info = */((SFSelect*)hk[Znr].eingabef)->activateCDKFselect(/*hk[Znr].eingabef, */0);
							break;
					}
		//#ifdef mdebug
		/*
			 mesg[0] = "<C>Letzte Eingabe:";
			 snprintf(temp, sizeof temp-1,"<C>(%.*s|%i)",(int)(sizeof(temp) - 10), info?info:"",Zweitzeichen);
		//		 if (info) if (hk[Znr].eingabef) if (hk[Znr].eingabef->info)
		//snprintf(temp,sizeof temp-1,"<C>(%.*s|%s|%i)",(int)(sizeof(temp)-10),info?info:"",info&&hk[Znr].eingabef&&hk[Znr].eingabef->info?hk[Znr].eingabef->info:"",Zweitzeichen);
		mesg[1] = temp;
		mesg[2] = "<C>Press any key to continue.";
		 */
		//#endif
		//EExitType	exitType=hk[Znr].eingabef->exitType;
		//		 popupLabel(cdkscreen,(CDK_CSTRING2) mesg, 3);
		// Tab
		//#ifdef richtig		 
		//		mvwprintw(cdkscreen->window,3,60,"Zweitzeichen: %i (%c)",Zweitzeichen,Zweitzeichen);
		refreshCDKWindow(cdkscreen->window);
		if (Zweitzeichen==-9) {
			Znr++;
			// Alt+Tab
			// Seite nach unten
		} else if (Zweitzeichen==-10) {
			Znr+=maxh-1;
		} else if (Zweitzeichen==-8) {
			Znr--;
			// Seite nach oben
		} else if (Zweitzeichen==-11) {
			Znr-=maxh-1;
			// Alt- +Buchstabe
		} else {
			//			mvwprintw(cdkscreen->window,1,30,"Zweitzeichen: %c",Zweitzeichen);
			if (Zweitzeichen) /*(info && *info==27)*/ {
				for(int aktent=0;aktent<maxhk;aktent++) {
					/*if (aktent<30)*/ 
					//					mvwprintw(cdkscreen->window,(aktent+1+Znr)%maxhk+yabst,100,"-->(%c) %i" ,hk[(aktent+1+Znr)%maxhk].buch,hk[(aktent+1+Znr)%maxhk].buch);
					refreshCDKWindow(cdkscreen->window);
					if (Zweitzeichen==hk[(aktent+1+Znr)%maxhk].buch ||
							((Zweitzeichen==194||Zweitzeichen==195)&&Zweitzeichen*256+Drittzeichen==hk[(aktent+1+Znr)%maxhk].buch)) {
						//						mvwprintw(cdkscreen->window,4,60,"buch: %c",hk[(aktent+1+Znr)%maxhk].buch);
						refreshCDKWindow(cdkscreen->window);
						Znr=(aktent+1+Znr)%maxhk;
						break;
					}
				}
				if (Znr==-1) 
					break;
			} else {
				break;
			}
		}
		if (Znr<0) {
			Znr=0; // Znr=sizeof hk/sizeof *hk-1;
		} else if (Znr>=maxhk) {
			Znr=maxhk-1; // sizeof hk/sizeof *hk) Znr=0;
		}
		//#endif 
	}  // while (1)
	for(int aktent=0;aktent<maxhk;aktent++) {
		//		 erg.push_back(hk[aktent].eingabef&&hk[aktent].eingabef->info?hk[aktent].eingabef->info:"");
		const char *ueb=
			hk[aktent].obalph==auswfld?
			((SAlphalist*)hk[aktent].eingabef)->entryField->efld.c_str():
			hk[aktent].obalph==dteifld?
			((SFSelect*)hk[aktent].eingabef)->entryField->efld.c_str():
			((SEntry*)hk[aktent].eingabef)->efld.c_str();
		erg.push_back(ueb);
	}
	/*
		 if (0) {
// Tell them what they typed.
if(exitType == vESCAPE_HIT && !Zweitzeichen) {
mesg[0] = "<C>Sie schlügen escape. No information passed back.";
mesg[1] = temp;
mesg[2] = "<C>Press any key to continue.";
for(t aktent=0;aktent<sizeof hk/sizeof *hk;aktent++) destroyCDKObject(hk[aktent].eingabef);
popupLabel(cdkscreen,(CDK_CSTRING2) mesg, 3);
} else if (exitType == vNORMAL) {
mesg[0] = "<C>Sie gaben folgendes ein";
sprintf(temp, "<C>(%.*s|%i)", (int)(sizeof(temp) - 10), info,Zweitzeichen);
mesg[1] = temp;
mesg[2] = "<C>Press any key to continue.";
for(int aktent=0;aktent<sizeof hk/sizeof *hk;aktent++) destroyCDKObject(hk[aktent].eingabef);
popupLabel(cdkscreen,(CDK_CSTRING2) mesg, 3);
} else {
sprintf(temp0,"Exit-type: %i",exitType);
mesg[0]=temp0;
mesg[1] = "<C>Sie gaben folgendes ein: ";
sprintf(temp, "<C>(%.*s|%i)",(int)(sizeof(temp) - 10), info,Zweitzeichen);
mesg[2] = temp;
mesg[3] = "<C>Eine Taste drücken zum Fortfahren.";
for(int aktent=0;aktent<sizeof hk/sizeof *hk;aktent++) destroyCDKObject(hk[aktent].eingabef);
popupLabel(cdkscreen,(CDK_CSTRING2) mesg, 4);
}
}
	 */

/* Clean up and exit. */
cdkscreen->destroyCDKScreen();
endCDK();
for(size_t aktent=0;aktent<erg.size();aktent++) {
	printf("%s\n",erg[aktent].c_str());
}
return EXIT_SUCCESS;
}

