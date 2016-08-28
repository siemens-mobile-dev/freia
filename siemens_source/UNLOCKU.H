//---------------------------------------------------------------------------

#ifndef unlockuH
#define unlockuH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "OvalBtn.hpp"
#include "UPlasmaForm.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class Tunlockingfunctions : public TForm
{
__published:	// IDE-managed Components
        TPlasmaForm *PlasmaForm1;
        TRadioGroup *unlockaction;
        TGroupBox *unlocksetup;
        TCheckBox *locktoprovider;
        TLabel *Label2;
        TMaskEdit *provider1;
        TMaskEdit *provider2;
        TCheckBox *autolocktoprovider;
        TLabel *Label1;
        TMaskEdit *imei;
        TCheckBox *updateflashIMEI;
        TOvalButton *OvalButton1;
        TOvalButton *OvalButton2;
        TOvalButton *OvalButton3;
        TOvalButton *OvalButton4;
        void __fastcall OvalButton6Click(TObject *Sender);
        void __fastcall OvalButton2Click(TObject *Sender);
        void __fastcall locktoproviderClick(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall autolocktoproviderClick(TObject *Sender);
        void __fastcall updateflashIMEIClick(TObject *Sender);
        void __fastcall unlockactionClick(TObject *Sender);
        void __fastcall OvalButton3Click(TObject *Sender);
        void __fastcall OvalButton4Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall Tunlockingfunctions(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tunlockingfunctions *unlockingfunctions;
//---------------------------------------------------------------------------
#endif
