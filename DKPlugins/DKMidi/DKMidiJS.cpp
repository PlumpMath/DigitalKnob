#ifdef USE_DKDuktape 
#include "DKMidi.h"
#include "DKMidiJS.h"


/////////////////////
void DKMidiJS::Init()
{
	DKDuktape::AttachFunction("DKMidi_GetMidiInputs", DKMidiJS::GetMidiInputs, 0);
	DKDuktape::AttachFunction("DKMidi_GetMidiOutputs", DKMidiJS::GetMidiOutputs, 0);
	DKDuktape::AttachFunction("DKMidi_ToggleMidiInput", DKMidiJS::ToggleMidiInput, 1);
	DKDuktape::AttachFunction("DKMidi_ToggleMidiOutput", DKMidiJS::ToggleMidiOutput, 1);
	DKDuktape::AttachFunction("DKMidi_SendMidi", DKMidiJS::SendMidi, 3);
}

//////////////////////////////////////
int DKMidiJS::GetMidiInputs(duk_context* ctx)
{
	DKMidi::Instance("DKMidi");
	DKStringArray inputs;
	DKMidi::Instance("DKMidi")->GetInputs(inputs);
	DKString final = toString(inputs,",");
	duk_push_string(ctx, final.c_str());
	return 1;
}

///////////////////////////////////////
int DKMidiJS::GetMidiOutputs(duk_context* ctx)
{
	DKMidi::Instance("DKMidi");
	DKStringArray outputs;
	DKMidi::Instance("DKMidi")->GetOutputs(outputs);
	DKString final = toString(outputs,",");
	duk_push_string(ctx, final.c_str());
	return 1;
}

////////////////////////////////////////
int DKMidiJS::ToggleMidiInput(duk_context* ctx)
{
	DKString input = duk_require_string(ctx, 0);
	if(DKMidi::Instance("DKMidi")->ToggleInput(input)){
		return 1;
	}
	return 0;
}

/////////////////////////////////////////
int DKMidiJS::ToggleMidiOutput(duk_context* ctx)
{
	DKString output = duk_require_string(ctx, 0);
	if(DKMidi::Instance("DKMidi")->ToggleOutput(output)){
		return 1;
	}
	return 0;
}

/////////////////////////////////
int DKMidiJS::SendMidi(duk_context* ctx)
{
	int var1 = duk_require_int(ctx, 0);
	int var2 = duk_require_int(ctx, 1);
	int var3 = duk_require_int(ctx, 2);

	std::vector<unsigned char> message;
	message.push_back(var1);
	message.push_back(var2);
	message.push_back(var3);
	DKMidi::Instance("DKMidi")->midiout->sendMessage(&message);
	return 1;
}


#endif //USE_DKDuktape