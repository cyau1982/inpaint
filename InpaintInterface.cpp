#include "InpaintInterface.h"
#include "InpaintParameters.h"
#include "InpaintProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

InpaintInterface* TheInpaintInterface = nullptr;

InpaintInterface::InpaintInterface()
	: instance(TheInpaintProcess)
{
	TheInpaintInterface = this;
}

InpaintInterface::~InpaintInterface()
{
	if (GUI != nullptr)
		delete GUI, GUI = nullptr;
}

IsoString InpaintInterface::Id() const
{
	return "Inpaint";
}

MetaProcess* InpaintInterface::Process() const
{
	return TheInpaintProcess;
}

IsoString InpaintInterface::IconImageSVG() const
{
	return TheInpaintProcess->IconImageSVG();
}

InterfaceFeatures InpaintInterface::Features() const
{
	return InterfaceFeature::Default;
}

void InpaintInterface::ApplyInstance() const
{
	instance.LaunchOnCurrentView();
}

void InpaintInterface::ResetInstance()
{
	InpaintInstance defaultInstance(TheInpaintProcess);
	ImportProcess(defaultInstance);
}

bool InpaintInterface::Launch(const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/)
{
	if (GUI == nullptr)
	{
		GUI = new GUIData(*this);
		SetWindowTitle("Inpaint");
		UpdateControls();
	}

	dynamic = false;
	return &P == TheInpaintProcess;
}

ProcessImplementation* InpaintInterface::NewProcess() const
{
	return new InpaintInstance(instance);
}

bool InpaintInterface::ValidateProcess(const ProcessImplementation& p, String& whyNot) const
{
	if (dynamic_cast<const InpaintInstance*>(&p) != nullptr)
		return true;
	whyNot = "Not a Inpaint instance.";
	return false;
}

bool InpaintInterface::RequiresInstanceValidation() const
{
	return true;
}

bool InpaintInterface::ImportProcess(const ProcessImplementation& p)
{
	instance.Assign(p);
	UpdateControls();
	return true;
}

void InpaintInterface::UpdateControls()
{
	GUI->InpaintBlackPoint_NumericControl.SetValue(instance.blackPoint);
	GUI->InpaintPrecision_NumericControl.SetValue(instance.precision);
}

void InpaintInterface::__EditValueUpdated(NumericEdit& sender, double value)
{
	if (sender == GUI->InpaintBlackPoint_NumericControl)
		instance.blackPoint = value;
	else if (sender == GUI->InpaintPrecision_NumericControl)
		instance.precision = value;
}

InpaintInterface::GUIData::GUIData(InpaintInterface& w)
{
	pcl::Font fnt = w.Font();
	int labelWidth1 = fnt.Width(String("Black point:") + 'T');
	int editWidth1 = fnt.Width(String('0', 12));
	int ui4 = w.LogicalPixelsToPhysical(4);

	InpaintBlackPoint_NumericControl.label.SetText("Black point:");
	InpaintBlackPoint_NumericControl.label.SetFixedWidth(labelWidth1);
	InpaintBlackPoint_NumericControl.slider.SetRange(0, 600);
	InpaintBlackPoint_NumericControl.slider.SetScaledMinWidth(300);
	InpaintBlackPoint_NumericControl.SetReal();
	InpaintBlackPoint_NumericControl.SetRange(TheInpaintBlackPointParameter->MinimumValue(), TheInpaintBlackPointParameter->MaximumValue());
	InpaintBlackPoint_NumericControl.SetPrecision(TheInpaintBlackPointParameter->Precision());
	InpaintBlackPoint_NumericControl.edit.SetFixedWidth(editWidth1);
	InpaintBlackPoint_NumericControl.SetToolTip("<p>Pixel equal to or below this value will be inpainted.</p>");
	InpaintBlackPoint_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & InpaintInterface::__EditValueUpdated, w);
	InpaintBlackPoint_Sizer.SetSpacing(4);
	InpaintBlackPoint_Sizer.Add(InpaintBlackPoint_NumericControl);
	InpaintBlackPoint_Sizer.AddStretch();

	InpaintPrecision_NumericControl.label.SetText("Precision:");
	InpaintPrecision_NumericControl.label.SetFixedWidth(labelWidth1);
	InpaintPrecision_NumericControl.slider.SetRange(0, 1000);
	InpaintPrecision_NumericControl.slider.SetScaledMinWidth(300);
	InpaintPrecision_NumericControl.SetInteger();
	InpaintPrecision_NumericControl.SetRange(TheInpaintPrecisionParameter->MinimumValue(), TheInpaintPrecisionParameter->MaximumValue());
	InpaintPrecision_NumericControl.edit.SetFixedWidth(editWidth1);
	InpaintPrecision_NumericControl.SetToolTip("<p>Precision of inpainting. The higher value, the more neighbor pixels will be considered in inpainting calculation.</p>");
	InpaintPrecision_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & InpaintInterface::__EditValueUpdated, w);
	InpaintPrecision_Sizer.SetSpacing(4);
	InpaintPrecision_Sizer.Add(InpaintPrecision_NumericControl);
	InpaintPrecision_Sizer.AddStretch();

	Global_Sizer.SetMargin(8);
	Global_Sizer.SetSpacing(4);
	Global_Sizer.Add(InpaintBlackPoint_Sizer);
	Global_Sizer.Add(InpaintPrecision_Sizer);

	w.SetSizer(Global_Sizer);

	w.EnsureLayoutUpdated();
	w.AdjustToContents();
	w.SetFixedSize();
}

}	// namespace pcl
