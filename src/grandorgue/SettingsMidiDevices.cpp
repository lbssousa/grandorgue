/*
 * GrandOrgue - free pipe organ simulator
 *
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2012 GrandOrgue contributors (see AUTHORS)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <wx/numdlg.h>

#include "SettingsMidiDevices.h"
#include "GOrgueMidi.h"
#include "GOrgueSound.h"
#include "GOrgueSettings.h"

BEGIN_EVENT_TABLE(SettingsMidiDevices, wxPanel)
	EVT_LISTBOX(ID_INDEVICES, SettingsMidiDevices::OnInDevicesClick)
	EVT_LISTBOX_DCLICK(ID_INDEVICES, SettingsMidiDevices::OnInDevicesDoubleClick)
	EVT_BUTTON(ID_INPROPERTIES, SettingsMidiDevices::OnInDevicesDoubleClick)
END_EVENT_TABLE()

SettingsMidiDevices::SettingsMidiDevices(GOrgueSound& sound, wxWindow* parent) :
	wxPanel(parent, wxID_ANY),
	m_Sound(sound)
{
	m_Sound.GetMidi().UpdateDevices();

	wxArrayString choices;
	std::map<wxString, int> list = m_Sound.GetMidi().GetInDevices();
	for (std::map<wxString, int>::iterator it2 = list.begin(); it2 != list.end(); it2++)
	{
		choices.push_back(it2->first);
		m_InDeviceData.push_back(m_Sound.GetSettings().GetMidiInDeviceChannelShift(it2->first));
	}

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* item3 = new wxStaticBoxSizer(wxVERTICAL, this, _("MIDI &input devices"));
	m_InDevices = new wxCheckListBox(this, ID_INDEVICES, wxDefaultPosition, wxDefaultSize, choices);
	for (unsigned i = 0; i < m_InDeviceData.size(); i++)
	{
		if (m_InDeviceData[i] < 0)
			m_InDeviceData[i] = -m_InDeviceData[i] - 1;
		else
			m_InDevices->Check(i);
	}
	item3->Add(m_InDevices, 1, wxEXPAND | wxALL, 5);
	m_InProperties = new wxButton(this, ID_INPROPERTIES, _("A&dvanced..."));
	m_InProperties->Disable();
	item3->Add(m_InProperties, 0, wxALIGN_RIGHT | wxALL, 5);
	topSizer->Add(item3, 1, wxEXPAND | wxALIGN_CENTER | wxALL, 5);

	choices.clear();
	std::vector<bool> out_state;
	list = m_Sound.GetMidi().GetOutDevices();
	for (std::map<wxString, int>::iterator it2 = list.begin(); it2 != list.end(); it2++)
	{
		choices.push_back(it2->first);
		out_state.push_back(m_Sound.GetSettings().GetMidiOutState(it2->first) == 1);
	}

	item3 = new wxStaticBoxSizer(wxVERTICAL, this, _("MIDI &output devices"));
	m_OutDevices = new wxCheckListBox(this, ID_OUTDEVICES, wxDefaultPosition, wxDefaultSize, choices);
	for (unsigned i = 0; i < out_state.size(); i++)
		if (out_state[i])
			m_OutDevices->Check(i);

	item3->Add(m_OutDevices, 1, wxEXPAND | wxALL, 5);
	topSizer->Add(item3, 1, wxEXPAND | wxALIGN_CENTER | wxALL, 5);

	topSizer->AddSpacer(5);
	this->SetSizer(topSizer);
	topSizer->Fit(this);
}

void SettingsMidiDevices::OnInDevicesClick(wxCommandEvent& event)
{
	m_InProperties->Enable();
}

void SettingsMidiDevices::OnInDevicesDoubleClick(wxCommandEvent& event)
{
	m_InProperties->Enable();
	int index = m_InDevices->GetSelection();
	int result = ::wxGetNumberFromUser(_("A channel offset allows the use of two MIDI\ninterfaces with conflicting MIDI channels. For\nexample, applying a channel offset of 8 to\none of the MIDI interfaces would cause that\ninterface's channel 1 to appear as channel 9,\nchannel 2 to appear as channel 10, and so on."), _("Channel offset:"), 
					   m_InDevices->GetString(index), m_InDeviceData[index], 0, 15, this);

	if (result >= 0)
		m_InDeviceData[index] = result;
}

void SettingsMidiDevices::Save()
{
	for (unsigned i = 0; i < m_InDevices->GetCount(); i++)
	{
		int j;

		j = m_InDeviceData[i];
		if (!m_InDevices->IsChecked(i))
			j = -j - 1;
		m_Sound.GetSettings().SetMidiInDeviceChannelShift(m_InDevices->GetString(i), j);
	}

	for (unsigned i = 0; i < m_OutDevices->GetCount(); i++)
	{
		m_Sound.GetSettings().SetMidiOutState(m_OutDevices->GetString(i), m_OutDevices->IsChecked(i));
	}
}
