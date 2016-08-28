//---------------------------------------------------------------------------

#ifndef configuH
#define configuH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "OvalBtn.hpp"
#include "UPlasmaForm.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class Tconfigurationfunctions : public TForm
{
__published:	// IDE-managed Components
        TPlasmaForm *PlasmaForm1;
        TRadioGroup *debuglevel;
        TRadioGroup *COMportapp;
        TRadioGroup *COMportdng;
        TRadioGroup *COMspeed;
        TOvalButton *OvalButton1;
        TOvalButton *OvalButton2;
        TGroupBox *GroupBox1;
        TCheckBox *com_rts;
        TCheckBox *com_dtr;
        TRadioGroup *boottype;
        void __fastcall OvalButton6Click(TObject *Sender);
        void __fastcall OvalButton2Click(TObject *Sender);
        void __fastcall COMportappClick(TObject *Sender);
        void __fastcall COMportdngClick(TObject *Sender);
        void __fastcall debuglevelClick(TObject *Sender);
        void __fastcall COMspeedClick(TObject *Sender);
        void __fastcall bootviabootcoreClick(TObject *Sender);
        void __fastcall com_rtsClick(TObject *Sender);
        void __fastcall com_dtrClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall Tconfigurationfunctions(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tconfigurationfunctions *configurationfunctions;
//---------------------------------------------------------------------------
#endif
