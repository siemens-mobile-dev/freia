//---------------------------------------------------------------------------

#ifndef flashuH
#define flashuH
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
class Tflashingfunctions : public TForm
{
__published:	// IDE-managed Components
        TPlasmaForm *PlasmaForm1;
        TRadioGroup *preset;
        TLabel *startaddr_l;
        TLabel *endaddr_l;
        TLabel *startoffs_l;
        TLabel *countfile_l;
        TEdit *startaddr;
        TEdit *endaddr;
        TEdit *startoffs;
        TEdit *countfile;
        TOvalButton *OvalButton1;
        TOvalButton *OvalButton2;
        void __fastcall OvalButton6Click(TObject *Sender);
        void __fastcall OvalButton2Click(TObject *Sender);
        void __fastcall presetClick(TObject *Sender);
        void __fastcall startoffsExit(TObject *Sender);
        void __fastcall countfileExit(TObject *Sender);
private:	// User declarations
        void __fastcall validatevalues (TObject * Sender);
        bool __fastcall GetSelectedAddress(int entry_index, long *startaddr, long *endaddr);        
public:		// User declarations
        __fastcall Tflashingfunctions(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tflashingfunctions *flashingfunctions;
//---------------------------------------------------------------------------
#endif
