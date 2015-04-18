#include "autoLayout.h"

using namespace AutoLayout;


Void autoLayout::clear(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e)
{
	delete myComponent;
}

Void autoLayout::button1_Click(System::Object^  sender, System::EventArgs^  e) 
{
	myComponent = new component;
	this->Controls->Remove(this->button1);
	clean();
	paraSet();
	polySet();
	powerSet();
	activeSet();
	nActiveSet();
	pselectSet();
	nselectSet();
	trackInitialize();
	channelRouting();
	drawLayout();
	MessageBox::Show(this, "end drawing!");
}

Void autoLayout::trackInitialize(void)
{
	track *fisrtTrack = new track(firstTrackPosition);// initially, there's one track already
	myComponent->tracks.push_back(fisrtTrack);
}

Void autoLayout::channelRouting(void)
{
	for(unsigned int i=0; i<myComponent->Top.size(); i++)// for each column, repeat
	{
		myComponent->aboutToFree.clear();
		step1(i);// bring two sides in
		step2(i);
		step3(i);
		step4(i);
		step5(i);// widen if needed
		step6(i);// extend to next
	}
}

Void autoLayout::step1(unsigned int n)
{
	if((myComponent->Top[n]->name == myComponent->Butt[n]->name) && myComponent->Top[n]->name != "0"
		&& (!myComponent->Butt[n]->poly))// the two is the same net
	{
		if(!checkIfNeededAnymore(myComponent->Top[n]->name, n))// it won't be used again
		{
			path *newPath = new path(myComponent->Top[n]->name, 
				myComponent->Top[n]->x, myComponent->Top[n]->y, myComponent->Butt[n]->x, myComponent->Butt[n]->y);// create a new path
			checkIfNetExist(newPath);// the net doesn't exit 
			// if exits, it is pushed into the net in the checkIfPathExist() function
		}
		else// find a legal track to contain this net
		{
			if(!checkIfNeedNewTrack(myComponent->Top[n]))
			{
				//MessageBox::Show(this, "bring it to the legal track");
				for(unsigned int i=0; i<myComponent->tracks.size(); i++)// search the nearest free track
				{
					if(myComponent->tracks[i]->assigned == false)
					{
						path *newPath = new path(myComponent->Top[n]->name, myComponent->Top[n]->x, 
							myComponent->Top[n]->y, myComponent->Butt[n]->x, myComponent->Butt[n]->y);
						checkIfNetExist(newPath);// the net doesn't exit 
						// if exits, it is pushed into the net in the checkIfPathExist() function
						myComponent->Top[i]->broughtIn = true;
						myComponent->Butt[i]->broughtIn = true;
						myComponent->tracks[i]->Assign(newPath->name);// it is assigned now, assigned by this net
						break;
					}
				}
			}
			else
			{
				//MessageBox::Show(this, "create a new track for both in step 5");
			}
		}
	}
	//
	// top and butt are not the same net
	else 
	{
		bool topAssigned;
		bool buttAssigned;
		if(myComponent->Top[n]->name != "VDD" && myComponent->Top[n]->name != "0" && (!myComponent->Top[n]->poly) )// it should be put into a track
		{
			topAssigned = checkTheTopNearest(myComponent->Top[n], n);
		}
		if(myComponent->Butt[n]->name != "GND" && myComponent->Butt[n]->name != "0" && (!myComponent->Butt[n]->poly) )// it should be put into a track
		{
			buttAssigned = checkTheButtNearest(myComponent->Butt[n], n);
		}
		if(topAssigned && buttAssigned)// if both are assigned now, it should go to next step
			return;
		else// try to assign them all
		{
			track *topChoice = new track;
			track *buttChoice = new track;
			if(!myComponent->Top[n]->broughtIn)
				topChoice = topToChoose(myComponent->Top[n]);// the top's choice
			if(!myComponent->Butt[n]->broughtIn)
				buttChoice = buttToChoose(myComponent->Butt[n]);;// the butt's choice

			if(!topChoice->isNull() && buttChoice->isNull())// top has a choice while butt hasn't
			{
				assignTopToTrack(myComponent->Top[n], topChoice, n);
				if(checkIfAlreadyInTrack(myComponent->Top[n]->name))
					topChoice->Assign(myComponent->Top[n]->name);
			}
			else if(topChoice->isNull() && !buttChoice->isNull())// butt has a choice while top has't
			{
				assignButtToTrack(myComponent->Butt[n], buttChoice, n);
				if(checkIfAlreadyInTrack(myComponent->Butt[n]->name))
					buttChoice->Assign(myComponent->Butt[n]->name);
			}
			else if(!topChoice->isNull() && !buttChoice->isNull())// both has a choice
			{
				if(topChoice->y < buttChoice->y)// won't merge
				{
					assignTopToTrack(myComponent->Top[n], topChoice, n);// assign both
					assignButtToTrack(myComponent->Butt[n], buttChoice, n);
					if(checkIfAlreadyInTrack(myComponent->Top[n]->name))
						topChoice->Assign(myComponent->Top[n]->name);
					if(checkIfAlreadyInTrack(myComponent->Butt[n]->name))
						buttChoice->Assign(myComponent->Butt[n]->name);
				}
				else// pick a shorter one
				{
					if(fabs(myComponent->Top[n]->y - topChoice->y) < fabs(myComponent->Butt[n]->y - buttChoice->y))
					{
						assignTopToTrack(myComponent->Top[n], topChoice, n);
						if(checkIfAlreadyInTrack(myComponent->Top[n]->name))
							topChoice->Assign(myComponent->Top[n]->name);
					}
					else
					{
						assignButtToTrack(myComponent->Butt[n], buttChoice, n);
						if(checkIfAlreadyInTrack(myComponent->Butt[n]->name))
							buttChoice->Assign(myComponent->Butt[n]->name);
					}
				}
			}
			else
			{
				return;// neither has a choice
			}
		}
	}
}

Void autoLayout::step2(unsigned int n)// reduce as many nets as possible
{
	vector<vector<track *> > pairs;// all pairs that are the same net
	for(unsigned int i=0; i<myComponent->tracks.size(); i++)// for each track
	{
		if(myComponent->tracks[i]->assigned)
		{
			for(unsigned int j=i+1; j<myComponent->tracks.size(); j++)// for other tracks
			{
				if(!checkIfIncluded(myComponent->tracks[j], pairs))
				{
					if(myComponent->tracks[i]->user == myComponent->tracks[j]->user && myComponent->tracks[i]->user != " ")
					{
						vector<track *> vtemp;
						vtemp.push_back(myComponent->tracks[i]);
						vtemp.push_back(myComponent->tracks[j]);
						pairs.push_back(vtemp);
					}
				}
			}
		}
	}
	int a = 0;
	if(pairs.size() > 0)
	{
		for(unsigned int i = 0; i<pairs.size(); i++)
		{
			for(unsigned int j=1; j<pairs[i].size(); j++)
				assignTrackToTrack(myComponent->Top[n], n, pairs[i][0], pairs[i][j]);
		}
	}
}

Void autoLayout::step3(unsigned int n)
{

}

Void autoLayout::step4(unsigned int n)
{

}

Void autoLayout::step5(unsigned int n)
{
	if(!myComponent->Top[n]->broughtIn && myComponent->Top[n]->name != "VDD" && 
		myComponent->Top[n]->name != "0" && !myComponent->Top[n]->poly)
		assignTopToNewTrack(myComponent->Top[n], n);
	if(!myComponent->Butt[n]->broughtIn && myComponent->Butt[n]->name != "GND" && 
		myComponent->Butt[n]->name != "0" && !myComponent->Butt[n]->poly)
		assignButtToNewTrack(myComponent->Butt[n], n);
}

Void autoLayout::step6(unsigned int n)// extend each track to next column
{
	vector<track *> vtmp;
	for(unsigned int i=0; i<myComponent->tracks.size(); i++)
	{
		for(unsigned int j=i+1; j<myComponent->tracks.size(); j++)
		{
			if((myComponent->tracks[i]->user == myComponent->tracks[j]->user) && (myComponent->tracks[i]->user!=" "))
			{
				vtmp.push_back(myComponent->tracks[i]);
				vtmp.push_back(myComponent->tracks[j]);
				break;
			}
		}
	}
	for(unsigned int i=0; i<myComponent->aboutToFree.size(); i++)// free these tracks
	{
		myComponent->aboutToFree[i]->Free();
	}
	for(unsigned int i=0; i<vtmp.size(); i++)
	{
		vtmp[i]->Assign(vtmp[i]->user);
	}
	if(n != myComponent->Top.size()-1)
	{
	for(unsigned int i=0; i<myComponent->tracks.size(); i++)
	{
		if(myComponent->tracks[i]->assigned == true)// for each assigned track(if the net is end, the track will be free)
		{
			for(unsigned int j=0; j<myComponent->net.size(); j++)//for each net
			{
				if(myComponent->net[j][0]->name == myComponent->tracks[i]->user)// find the right net
				{
					int cont = 0;
					for(unsigned int k=0; k<myComponent->net[j].size(); k++)// for each path
					{
						if(fabs(myComponent->net[j][k]->endX - myComponent->Top[n]->x)<0.1 && 
							myComponent->net[j][k]->M1orM2 == 2 &&
							myComponent->net[j][k]->endY == myComponent->tracks[i]->y)// find the right path to extend
						{
							myComponent->net[j][k]->endX = myComponent->Top[n+1]->x;
							cont++;
						}
					}
					if(cont == 0)// there's no horizontal path, then create a new one
					{
						path *newPath = new path(myComponent->net[j][0]->name, myComponent->Top[n]->x,
							myComponent->tracks[i]->y, myComponent->Top[n+1]->x, myComponent->tracks[i]->y);
						myComponent->net[j].push_back(newPath);// put int the net
						createViaM1ToM2(newPath, 0);
					}
				}
			}
		}
	}
	}
}

Void autoLayout::generateVia(float a, float b)
{
	material vtmp1("M1ToM2", M1ToM2, (float)(a - lambda), (float)(b - lambda), (float)(2 * lambda), (float)(2 * lambda));
	myComponent->via.push_back(vtmp1);
	material vtmp2("M1ToM2", METAL1, (float)(a - 2 * lambda), (float)(b - 2 * lambda), (float)(4 * lambda), (float)(4 * lambda));
	myComponent->m1Select.push_back(vtmp2);
	material vtmp3("M1ToM2", METAL2, (float)(a - 2 * lambda), (float)(b - 2 * lambda), (float)(4 * lambda), (float)(4 * lambda));
	myComponent->m2Select.push_back(vtmp3);
}

Void autoLayout::createViaM1ToM2(path *p, int dir)// 0: top, 1: buttom, 2: both side
{
	switch (dir)
	{
	case 0:
		generateVia(p->startX, p->startY);
		break;

	case 1:
		generateVia(p->startX, p->endY);
		break;

	case 2:
		generateVia(p->startX, p->startY);
		generateVia(p->endX, p->endY);
		break;

	default:
		break;
	}
}

bool autoLayout::checkIfRaising(column *c, unsigned int n, track *t)
{
	if(n + netSteady < myComponent->Top.size())
	{
		for(unsigned int i=n+1; i<myComponent->Top.size(); i++)
		{
			if(myComponent->Top[i]->name == t->user)// find it in top
			{
				for(unsigned int j = i+1; j<netSteady; j++)
				{
					if(j >= myComponent->Top.size() - 1)
						return false;
					if(myComponent->Butt[j]->name == t->user)// find it is not steady
						return false;
				}
			return true;
			}
		}
	}
	return false;
}

Void autoLayout::assignTrackToTrack(column *c, unsigned int n, track *t1, track *t2)
{
	if(c->poly)
		return;// abort
	for(unsigned int i=0; i<c->MergeSpace.size(); i++)// check each merged region
	{
		if(t1->y > (c->MergeSpace[i][0]+0.1) && t1->y < (c->MergeSpace[i][1]-0.1))
		{
			return;// abort
		}
		if(t2->y > (c->MergeSpace[i][0]+0.1) && t2->y < (c->MergeSpace[i][1]-0.1))
		{
			return;// abort
		}
		if(t1->y < (c->MergeSpace[i][0]-0.1) && t2->y > (c->MergeSpace[i][1]+0.1))
		{
			return;
		}
	}
	path *newPath = new path(t1->user, c->x, t1->y, c->x, t2->y);// create new path
	bool newTop = false;
	bool newButt = false;
	for(unsigned int i=0; i<myComponent->net.size(); i++)// find in the nets
	{
		if(myComponent->net[i][0]->name == newPath->name)//find the paths with same name
		{
			for(unsigned int j=0; j<myComponent->net[i].size(); j++)
			{
				if(fabs(myComponent->net[i][j]->startX - newPath->startX) < 0.2)
				{
					if(fabs(myComponent->net[i][j]->endY - newPath->startY) < 0.2)// connected to one end
					{
						myComponent->net[i][j]->endY = newPath->endY;
						newPath = myComponent->net[i][j];
						newButt = true;
					}
					if(fabs(myComponent->net[i][j]->startY - newPath->endY) < 0.2)// connected to one start
					{
						checkIfDeleteVia(c, myComponent->net[i][j]->name, myComponent->net[i][j]->startY);
						myComponent->net[i][j]->startY = newPath->startY;
						newPath = myComponent->net[i][j];
						newTop = true;
					}
				}
			}
		}
	}
	if(!newButt && !newTop)// it's not extended
	{
		checkIfNetExist(newPath);
		createViaM1ToM2(newPath, 2);
	}
	if(newTop)// it's not extended
	{
		createViaM1ToM2(newPath, 0);
	}
	if(newButt)// it's not extended
	{
		createViaM1ToM2(newPath, 1);
	}
	vector<float> merge;
	merge.push_back(t1->y);
	merge.push_back(t2->y);
	myComponent->Top[n]->MergeSpace.push_back(merge);
	myComponent->Butt[n]->MergeSpace.push_back(merge);
	if(!checkIfNeededAnymore(t1->user, n))
	{
		t1->Free();
		t2->Free();
	}
	else
	{
		if(checkIfRaising(c, n, t1))// find it is raising
		{
			if(t1->y < t2->y)// t1 is higher
			{
				t1->Assign(newPath->name);// it is assigned now, assigned by this net
				t2->Free();
			}
			else
			{
				t2->Assign(newPath->name);// it is assigned now, assigned by this net
				t1->Free();
			}
		}
		else
		{
			if(t1->y > t2->y)// t2 is higher
			{
				t1->Assign(newPath->name);// it is assigned now, assigned by this net
				t2->Free();
			}
			else
			{
				t2->Assign(newPath->name);// it is assigned now, assigned by this net
				t1->Free();
			}
		}
	}
}

bool autoLayout::checkIfDeleteVia(column *c, string s, float yPosition)
{
	bool need = true;
	for(unsigned int i=0; i<myComponent->net.size(); i++)
	{
		if(myComponent->net[i][0]->name == s)// find the net
		{
			for(unsigned int j=0; j<myComponent->net[i].size(); j++)
			{
				if(myComponent->net[i][j]->M1orM2 == 2)// find metal2
				{
					if(fabs(myComponent->net[i][j]->endX - c->x) < 0.2)// it is end here
					{
						if(fabs(myComponent->net[i][j]->startY - yPosition) < 0.2)// it is in this track
						{
							need = false;// find it, it don't need to be deleted
							return false;
						}
					}
				}
			}
		}
	}
	for(unsigned int i=0; i<myComponent->via.size(); i++)
	{
		if(fabs(myComponent->via[i].startX - c->x) < 3 && fabs(myComponent->via[i].startY - yPosition) < 3)
			myComponent->via.erase(myComponent->via.begin() + i);
	}
	for(unsigned int i=0; i<myComponent->m1Select.size(); i++)
	{
		if(fabs(myComponent->m1Select[i].startX - c->x) < 3 && fabs(myComponent->m1Select[i].startY - yPosition) < 3)
			myComponent->m1Select.erase(myComponent->m1Select.begin() + i);
	}
	for(unsigned int i=0; i<myComponent->m2Select.size(); i++)
	{
		if(fabs(myComponent->m2Select[i].startX - c->x) < 3 && fabs(myComponent->m2Select[i].startY - yPosition) < 3)
			myComponent->m2Select.erase(myComponent->m2Select.begin() + i);
	}
	return true;
}

Void autoLayout::assignTopToTrack(column *c, track *t, unsigned int n)
{
	path *newPath = new path(c->name, c->x, c->y, c->x, t->y);// create new path
	checkIfNetExist(newPath);// check if this path belongs to any net exist
	c->broughtIn = true;
	vector<float> merge;
	merge.push_back(c->y);
	merge.push_back(t->y);
	c->MergeSpace.push_back(merge);
	myComponent->Butt[n]->MergeSpace.push_back(merge);
	if(!checkIfNeededAnymore(c->name, n))
		//myComponent->aboutToFree.push_back(t);
			t->Free();
	else
	t->Assign(newPath->name);// it is assigned now, assigned by this net
	//FillPath(newPath);// print it temporary
	createViaM1ToM2(newPath, 1);
}

Void autoLayout::assignButtToTrack(column *c, track *t, unsigned int n)
{
	path *newPath = new path(c->name, c->x, t->y, c->x, c->y);// create new path
	checkIfNetExist(newPath);// check if this path belongs to any net exist
	c->broughtIn = true;
	vector<float> merge;
	merge.push_back(t->y);
	merge.push_back(c->y);
	c->MergeSpace.push_back(merge);
	myComponent->Top[n]->MergeSpace.push_back(merge);
	if(!checkIfNeededAnymore(c->name, n))
		t->Free();
	else
		t->Assign(newPath->name);// it is assigned now, assigned by this net
	//FillPath(newPath);// print it temporary
	createViaM1ToM2(newPath, 0);
}

Void autoLayout::assignTopToNewTrack(column *c, unsigned int n)
{
	//MessageBox::Show(this, "creating new track for top!");
	track *newTrack = createNewTrack(c);
	path *newPath = new path(c->name, c->x, c->y, c->x, newTrack->y);// create new path
	checkIfNetExist(newPath);// check if this path belongs to any net exist
	c->broughtIn = true;
	vector<float> merge;
	merge.push_back(c->y);
	merge.push_back(newTrack->y);
	c->MergeSpace.push_back(merge);
	myComponent->Butt[n]->MergeSpace.push_back(merge);
	if(!checkIfNeededAnymore(c->name, n))
		//myComponent->aboutToFree.push_back(newTrack);
			newTrack->Free();
	else
	newTrack->Assign(newPath->name);// it is assigned now, assigned by this net
	//FillPath(newPath);// print it temporary
	if(checkIfAlreadyInTrack(c->name))
		newTrack->Assign(c->name);
	createViaM1ToM2(newPath, 1);

}

Void autoLayout::assignButtToNewTrack(column *c, unsigned int n)
{
	//MessageBox::Show(this, "creating new track for buttom!");
	track *newTrack = createNewTrack(c);
	path *newPath = new path(c->name, c->x, newTrack->y, c->x, c->y);// create new path
	checkIfNetExist(newPath);// check if this path belongs to any net exist
	c->broughtIn = true;
	vector<float> merge;
	merge.push_back(newTrack->y);
	merge.push_back(c->y);
	c->MergeSpace.push_back(merge);
	myComponent->Top[n]->MergeSpace.push_back(merge);
	if(!checkIfNeededAnymore(c->name, n))
		//myComponent->aboutToFree.push_back(newTrack);
			newTrack->Free();
	else
	newTrack->Assign(newPath->name);// it is assigned now, assigned by this net
	//FillPath(newPath);// print it temporary
	if(checkIfAlreadyInTrack(c->name))
		newTrack->Assign(c->name);
	createViaM1ToM2(newPath, 0);
}

track *autoLayout::createNewTrack(column *c)
{
	float minMerge = formH;
	float maxMerge = 0;
	unsigned int position;
	if(c->MergeSpace.size() == 0)
		position = (myComponent->tracks.size() + 1)/2;// let the new track as in the middle as possible
	else
	{
		for(unsigned int i=0; i<c->MergeSpace.size(); i++)
		{
			if(c->MergeSpace[i][0] < minMerge)
				minMerge = c->MergeSpace[i][0];
			if(c->MergeSpace[i][1] < minMerge)
				minMerge = c->MergeSpace[i][1];
			if(c->MergeSpace[i][0] > maxMerge)
				maxMerge = c->MergeSpace[i][0];
			if(c->MergeSpace[i][1] > maxMerge)
				maxMerge = c->MergeSpace[i][1];
		}
		if(c->y > maxMerge)
		{
			position = myComponent->tracks.size();// let the new track as in the middle as possible
		}
		if(c->y < minMerge)
		{
			position = 1;// let the new track as in the middle as possible
		}
	}
	float yValue = (float)(firstTrackPosition + position * 7 * lambda);
	for(unsigned int i=0; i<myComponent->tracks.size(); i++)
	{
		if(myComponent->tracks[i]->y > yValue - 2)
			myComponent->tracks[i]->y += trackGap;
	}
	track *newTrack = new track(yValue);
	myComponent->tracks.push_back(newTrack);
	refreshPara(newTrack);
	return newTrack;
}

bool autoLayout::checkIfIncluded(track *t, vector<vector<track *> > &v)
{
	if(v.size() == 0)
		return false;
	else
	{
		for(unsigned int i=0; i<v.size(); i++)
		{
			if(t->user == v[i][0]->user)
				return true;
		}
		return false;
	}
}

Void autoLayout::refreshPara(track *temp)// this track is the divider
{
	float delta = (float)((side + 7 * myComponent->tracks.size() + 22) * lambda) - myComponent->height;
	myComponent->height = (float)((side + 7 * myComponent->tracks.size() + 22) * lambda);
	myComponent->ratio = (int)min(formW / myComponent->width, formH / myComponent->height);// amplify ratio
	offsetX = (formW - myComponent->ratio * myComponent->width) / 2;
	offsetY = (formH - myComponent->ratio * myComponent->height) / 2;
	for(unsigned int i=0; i<myComponent->poly.size(); i++)
	{
		myComponent->poly[i].H += delta;
	}
	for(unsigned int i=0; i<myComponent->metal1.size(); i++)
	{
		if(myComponent->metal1[i].startY > myComponent->tracks[0]->y)
			myComponent->metal1[i].startY += delta;
	}
	for(unsigned int i=0; i<myComponent->via.size(); i++)
	{
		if(myComponent->via[i].startY > temp->y - 3)
			myComponent->via[i].startY += delta;
	}
	for(unsigned int i=0; i<myComponent->m1Select.size(); i++)
	{
		if(myComponent->m1Select[i].startY > temp->y - 3)
			myComponent->m1Select[i].startY += delta;
	}
	for(unsigned int i=0; i<myComponent->m2Select.size(); i++)
	{
		if(myComponent->m2Select[i].startY > temp->y - 3)
			myComponent->m2Select[i].startY += delta;
	}
	for(unsigned int i=0; i<myComponent->nselect.size(); i++)
	{
		if(myComponent->nselect[i].startY > myComponent->tracks[0]->y)
			myComponent->nselect[i].startY += delta;
	}
	for(unsigned int i=0; i<myComponent->pselect.size(); i++)
	{
		if(myComponent->pselect[i].startY > myComponent->tracks[0]->y)
			myComponent->pselect[i].startY += delta;
	}
	for(unsigned int i=0; i<myComponent->activeN.size(); i++)
	{
		if(myComponent->activeN[i].startY > myComponent->tracks[0]->y)
			myComponent->activeN[i].startY += delta;
	}
	for(unsigned int i=0; i<myComponent->Butt.size(); i++)
	{
		if(myComponent->Butt[i]->y > myComponent->tracks[0]->y)
			myComponent->Butt[i]->y += delta;
	}
	for(unsigned int i=0; i<myComponent->net.size(); i++)
	{
		for(unsigned int j=0; j<myComponent->net[i].size(); j++)
		{
			if(myComponent->net[i][j]->startY > temp->y-3)
				myComponent->net[i][j]->startY += delta;
			if(myComponent->net[i][j]->endY > temp->y-3)
				myComponent->net[i][j]->endY += delta;
		}
	}
}

Void autoLayout::clean()
{
	Graphics ^dc =  pictureBox1->CreateGraphics();
	dc->Clear(Color::Black);
}

Void autoLayout::drawAllPath()
{
	for(unsigned int i=0; i<myComponent->net.size(); i++)
	{
		for(unsigned int j=0; j<myComponent->net[i].size(); j++)
		{
			FillPath(myComponent->net[i][j]);
		}
	}
}

bool autoLayout::checkTheTopNearest(column *c, unsigned int n)
{
	if(myComponent->tracks.size() == 0)// there's no track now
		return false;
	else if(myComponent->tracks[0]->assigned == false || myComponent->tracks[0]->user == c->name)
	{
		assignTopToTrack(c, myComponent->tracks[0], n);
		//if(!checkIfNeededAnymore(c->name, n))
		//	myComponent->aboutToFree.push_back(myComponent->tracks[0]);
		if(checkIfAlreadyInTrack(c->name))
			myComponent->tracks[0]->Assign(c->name);
		return true;
	}
	else
		return false;
}

bool autoLayout::checkTheButtNearest(column *c, unsigned int n)
{
	if(myComponent->tracks.size() == 0)// there's no track now
		return false;
	else 
	{
		track *buttTrack = findTheButtTrack(c);
		if(buttTrack->assigned == false || buttTrack->user == c->name)
		{
			assignButtToTrack(c, buttTrack, n);
			if(checkIfAlreadyInTrack(c->name))
				buttTrack->Assign(c->name);
			return true;
		}
		else
			return false;
	}
}

track *autoLayout::findTheButtTrack(column *c)
{
	float ytmp = 0;
	track *trackTmp = new track;
	for(unsigned int i = 0; i<myComponent->tracks.size(); i++)
	{
		if(myComponent->tracks[i]->y > ytmp)
		{
			ytmp = myComponent->tracks[i]->y;
			trackTmp = myComponent->tracks[i];// find the buttom track
		}
	}
	return trackTmp;
}

track *autoLayout::topToChoose(column *c)
{
	float ytmp = formH;
	track *trackTmp = new track;
	if(c->name != "VDD" && c->name != "0" && (!c->poly))
	{
		for(unsigned int i = 0; i<myComponent->tracks.size(); i++)
		{
			if(!checkIfMerged(c, myComponent->tracks[i]))
			{
				if((myComponent->tracks[i]->y < ytmp) && (!myComponent->tracks[i]->assigned || myComponent->tracks[i]->user == c->name))
				{
					ytmp = myComponent->tracks[i]->y;
					trackTmp = myComponent->tracks[i];// find the nearest legal track
				}
			}
		}
	}
	return trackTmp;
}

track *autoLayout::buttToChoose(column *c)
{
	float ytmp = 0;
	track *trackTmp = new track;
	if(c->name != "GND" && c->name != "0" && (!c->poly))
	{
		for(unsigned int i = 0; i<myComponent->tracks.size(); i++)
		{
			if(!checkIfMerged(c, myComponent->tracks[i]))
			{
				if((myComponent->tracks[i]->y > ytmp) && (!myComponent->tracks[i]->assigned || myComponent->tracks[i]->user == c->name))
				{
					ytmp = myComponent->tracks[i]->y;
					trackTmp = myComponent->tracks[i];// find the nearest legal track
				}
			}
		}
	}
	return trackTmp;
}

bool autoLayout::checkIfMerged(column *c, track *t)
{
	for(unsigned int i=0; i<c->MergeSpace.size(); i++)
	{
		if((t->y > c->MergeSpace[i][0]-2) && (t->y < c->MergeSpace[i][1] + 2))
			return true;
	}
	return false;
}

bool autoLayout::checkIfNeedNewTrack(column *c)
{
	if(myComponent->tracks.size() == 0)// there's no track now
		return true;
	else
	{
		for(unsigned int i=0; i<myComponent->tracks.size(); i++)// all tracks are assigned
		{
			if(myComponent->tracks[i]->assigned == false)// there's a free track
			{
				return false;
			}
			else if(myComponent->tracks[i]->user == c->name)// there's a track already assigned by the same net
			{
				return false;
			}
		}
		return true;// there's no free track
	}
}

bool autoLayout::checkIfNeededAnymore(string s, unsigned int n)
{
	if(n == myComponent->Top.size()-1)// it is the last column
		return false;
	for(unsigned int i=n+1; i<myComponent->Top.size(); i++)// check all the following columns
	{
		if(myComponent->Top[i]->name == s)// find it is still used in Pmos
			return true;
		if(myComponent->Butt[i]->name == s)// find it is still used in Nmos
			return true;
	}
	return false;
}

bool autoLayout::checkIfAlreadyInTrack(string s)
{
	for(unsigned int i=0; i<myComponent->tracks.size(); i++)
	{
		if(myComponent->tracks[i]->user == s)
		{
			return true; // find a same net in the tracks
		}
	}
	return false;
}

bool autoLayout::checkIfNetExist(path *p)
{
	if(myComponent->net.size() == 0)// there's no net yet
	{
		vector<path*> vtmp;
		vtmp.push_back(p);
		myComponent->net.push_back(vtmp);
		return false;
	}
	else
	{
		for(unsigned int i=0; i<myComponent->net.size(); i++)
		{
			if(myComponent->net[i][0]->name == p->name)// this path belongs to this net
			{
				myComponent->net[i].push_back(p);// put this path into its net
				return true;
			}
		}
		vector<path*> vtmp;
		vtmp.push_back(p);
		myComponent->net.push_back(vtmp);
		return false;// this path doesn't exist
	}
}

//Void autoLayout::createContact(void)
//{
//
//}

Void autoLayout::pselectSet(void)
{
	material tmp("pselect", PSELECT, (float)(4*lambda), (float)(12*lambda), 
		(float)((myComponent->width - 8*lambda)), (float)(14*lambda));
	myComponent->pselect.push_back(tmp);
}

Void autoLayout::nselectSet(void)
{
	material tmp("nselect", NSELECT, (float)(4*lambda), (float)((myComponent->height - 16) * lambda), 
		(float)((myComponent->width - 8*lambda)), (float)(9*lambda));
	myComponent->nselect.push_back(tmp);
}

Void autoLayout::polySet(void)
{
	for(unsigned int i=0; i<myComponent->expr.size(); i++)
	{
		if(myComponent->expr[i] != '%')
		{
			material tmp(myComponent->expr.substr(i,1), POLY, (float)((6+5)*lambda), 
				(float)(12*lambda), (float)(2*lambda), (float)((myComponent->height - side)*lambda));
			myComponent->poly.push_back(tmp);
		}
	}
}

Void autoLayout::activeSet(void)
{
	vector<material> temp;
	for(unsigned int i=0, j=0; i<myComponent->expr.size(); i++)
	{
		if(myComponent->expr[i] == '%')
		{
			myComponent->transistors.push_back(temp);
			temp.clear();
		}
		else
		{
			temp.push_back(myComponent->poly[j]);	
			j++;
		}
	}
	myComponent->transistors.push_back(temp);
	temp.clear();
	for(unsigned int i = 0, j = 0; i<myComponent->transistors.size(); i++)
	{
		if( i == 0)
		{
			material tmp1("active", ACTIVE, (float)(6 * lambda), (float)(14 * lambda ), 
				(float)((4 + 8 * myComponent->transistors[0].size()) * lambda), (float)(10 * lambda) );
			material tmp2("active", ACTIVE, (float)(6 * lambda), (float)((myComponent->height - 14) * lambda ), 
				(float)((4 + 8 * myComponent->transistors[0].size()) * lambda), (float)(5 * lambda ));
			myComponent->activeP.push_back(tmp1);
			myComponent->activeN.push_back(tmp2);
		}
		else
		{
			material tmp1("active", ACTIVE, (float)(myComponent->activeP[i-1].startX + myComponent->activeP[i-1].W + metalToMetal * lambda),
				(float)(14 * lambda), (float)((4 + 8 * myComponent->transistors[i].size()) * lambda), float(10 * lambda));
			material tmp2("active", ACTIVE, (float)(myComponent->activeN[i-1].startX + myComponent->activeN[i-1].W + metalToMetal * lambda),
				(float)((myComponent->height - 14) * lambda), (float)((4 + 8 * myComponent->transistors[i].size()) * lambda), float(5 * lambda));
			myComponent->activeP.push_back(tmp1);
			myComponent->activeN.push_back(tmp2);
		}
		for(unsigned int k = 0; k<myComponent->transistors[i].size(); k++, j++)
		{
			myComponent->poly[j].startX = myComponent->activeP[i].startX + (5 + 8 * k) * lambda;
		
			if(checkIfNeedPVia(parent->exp->PnetOrder[j][1]))// check P drain
			{
				material pvtmp1("via", VIA, (float)(myComponent->poly[j].startX - 4*lambda), (float)(myComponent->activeP[i].startY + 1.5 * lambda),
					(float)(2*lambda), (float)(2*lambda));
				material pvtmp2("via", VIA, (float)(myComponent->poly[j].startX - 4*lambda), (float)(myComponent->activeP[i].startY + 6.5 * lambda),
					(float)(2*lambda), (float)(2*lambda));
				myComponent->via.push_back(pvtmp1);
				myComponent->via.push_back(pvtmp2);
				if(k == 0)
				{
					column *ctmp = new column(parent->exp->PnetOrder[j][1], false, (float)(myComponent->poly[j].startX - 3*lambda), (float)(24 * lambda));
					myComponent->Top.push_back(ctmp);
					if(checkIfPower(parent->exp->PnetOrder[j][1]))
					{
						material vddtmp("vdd!", METAL1, (float)(myComponent->poly[j].startX - 5*lambda), (float)(5 * lambda),
							(float)(4*lambda), (float)(19*lambda));
						myComponent->metal1.push_back(vddtmp);
					}
					else
					{
						material vddtmp(" ", METAL1, (float)(myComponent->poly[j].startX - 5*lambda), (float)(myComponent->activeP[i].startY),
							(float)(4*lambda), (float)(10*lambda));
						myComponent->metal1.push_back(vddtmp);
					}
				}
			}
			else
			{
				if(k == 0)
				{
					column *ctmp = new column("0", false, (float)(myComponent->poly[j].startX - 3*lambda), (float)(24 * lambda));
					myComponent->Top.push_back(ctmp);
				}
			}

			//// add poly to column
			column *ctmp = new column(myComponent->poly[j].name, true, (float)(myComponent->poly[j].startX + lambda), (float)(24 * lambda));
			myComponent->Top.push_back(ctmp);

			if(checkIfNeedPVia(parent->exp->PnetOrder[j][2]))// check P source
			{
				material pvtmp1("via", VIA, (float)(myComponent->poly[j].startX + 4*lambda), (float)(myComponent->activeP[i].startY + 1.5 * lambda),
					(float)(2*lambda), (float)(2*lambda));
				material pvtmp2("via", VIA, (float)(myComponent->poly[j].startX + 4*lambda), (float)(myComponent->activeP[i].startY + 6.5 * lambda),
					(float)(2*lambda), (float)(2*lambda));
				myComponent->via.push_back(pvtmp1);
				myComponent->via.push_back(pvtmp2);
				column *ctmp = new column(parent->exp->PnetOrder[j][2], false, (float)(myComponent->poly[j].startX + 5*lambda), (float)(24 * lambda));
				myComponent->Top.push_back(ctmp);
				if(checkIfPower(parent->exp->PnetOrder[j][2]))
				{
					material vddtmp("vdd!", METAL1, (float)(myComponent->poly[j].startX + 3*lambda), (float)(5 * lambda),
						(float)(4*lambda), (float)(19*lambda));
					myComponent->metal1.push_back(vddtmp);
				}
				else
				{
					material vddtmp(" ", METAL1, (float)(myComponent->poly[j].startX + 3*lambda), (float)(myComponent->activeP[i].startY),
						(float)(4*lambda), (float)(10*lambda));
					myComponent->metal1.push_back(vddtmp);
				}
			}
			else
			{
				column *ctmp = new column("0", false, (float)(myComponent->poly[j].startX + 5*lambda), (float)(24 * lambda));
				myComponent->Top.push_back(ctmp);
			}

			///////////////////////////////////			Nmos		////////////////////////////////
			if(checkIfNeedNVia(parent->exp->NnetOrder[j][1]))// check N drain
			{
				material nvtmp1("via", VIA, (float)(myComponent->poly[j].startX - 4*lambda), (float)(myComponent->activeN[i].startY + 1.5 * lambda),
					(float)(2*lambda), (float)(2*lambda));
				myComponent->via.push_back(nvtmp1);
				if(k == 0)
				{
					column *ctmp = new column(parent->exp->NnetOrder[j][1], false, (float)(myComponent->poly[j].startX - 3*lambda), (float)((myComponent->height - 14) * lambda));
					myComponent->Butt.push_back(ctmp);
					if(checkIfPower(parent->exp->NnetOrder[j][1]))
					{
						material vddtmp("gnd!", METAL1, (float)(myComponent->poly[j].startX - 5*lambda), (float)((myComponent->height - 14) * lambda),
							(float)(4*lambda), (float)(14*lambda));
						myComponent->metal1.push_back(vddtmp);
					}
					else
					{
						material vddtmp(" ", METAL1, (float)(myComponent->poly[j].startX - 5*lambda), (float)((myComponent->height - 14) * lambda),
							(float)(4*lambda), (float)(5*lambda));
						myComponent->metal1.push_back(vddtmp);
					}
				}
			}
			else
			{
				if(k == 0)
				{
					column *ctmp = new column("0", false, (float)(myComponent->poly[j].startX - 3*lambda), (float)((myComponent->height - 14) * lambda));
					myComponent->Butt.push_back(ctmp);
				}
			}

			//// add poly to column
			column *ctmp2 = new column(myComponent->poly[j].name, true, (float)(myComponent->poly[j].startX + lambda), (float)((myComponent->height - 14) * lambda));
			myComponent->Butt.push_back(ctmp2);

			if(checkIfNeedNVia(parent->exp->NnetOrder[j][2]))// check P source
			{
				material nvtmp1("via", VIA, (float)(myComponent->poly[j].startX + 4*lambda), (float)(myComponent->activeN[i].startY + 1.5 * lambda),
					(float)(2*lambda), (float)(2*lambda));
				myComponent->via.push_back(nvtmp1);
				column *ctmp = new column(parent->exp->NnetOrder[j][2], false, (float)(myComponent->poly[j].startX + 5*lambda), (float)((myComponent->height - 14) * lambda));
				myComponent->Butt.push_back(ctmp);
				if(checkIfPower(parent->exp->NnetOrder[j][2]))
				{
					material vddtmp("gnd!", METAL1, (float)(myComponent->poly[j].startX + 3*lambda), (float)((myComponent->height - 14) * lambda),
						(float)(4*lambda), (float)(14*lambda));
					myComponent->metal1.push_back(vddtmp);
				}
				else
				{
					material vddtmp(" ", METAL1, (float)(myComponent->poly[j].startX + 3*lambda), (float)((myComponent->height - 14) * lambda),
						(float)(4*lambda), (float)(5*lambda));
					myComponent->metal1.push_back(vddtmp);
				}
			}
			else
			{
				column *ctmp = new column("0", false, (float)(myComponent->poly[j].startX + 5*lambda), (float)((myComponent->height - 14) * lambda));
				myComponent->Butt.push_back(ctmp);
			}
		
		}
	}
}

bool autoLayout::checkIfPower(string s)
{
	if(s == "VDD" || s == "GND")
		return true;
	else
		return false;
}

bool autoLayout::checkIfNeedPVia(string s)
{
	if(s == "OUT" || s == "VDD" || s == "GND")
		return true;
	int cont = 0;
	for(unsigned int i=0; i<parent->exp->Pnet.size(); i++)
	{
		for(unsigned int j=0; j<parent->exp->Pnet[i].size(); j++)
		{
			if(parent->exp->Pnet[i][j] == s)
				cont++;
			if(cont > 2)
				return true;// if cont > 2, it should be a via
		}
	}
	return false;
}

bool autoLayout::checkIfNeedNVia(string s)
{
	if(s == "OUT" || s == "VDD" || s == "GND")
		return true;
	int cont = 0;
	for(unsigned int i=0; i<parent->exp->Nnet.size(); i++)
	{
		for(unsigned int j=0; j<parent->exp->Nnet[i].size(); j++)
		{
			if(parent->exp->Nnet[i][j] == s)
				cont++;
			if(cont > 2)
				return true;// if cont > 2, it should be a via
		}
	}
	return false;
}

Void autoLayout::powerSet(void)
{
	material vdd("vdd!", METAL1, 0, (float)(5*lambda), 
		(float)(myComponent->width ), (float)(6));
	material gnd("gnd!", METAL1, 0, (float)((myComponent->height - 6)*lambda), 
		(float)(myComponent->width), (float)(6));
	myComponent->metal1.push_back(vdd);
	myComponent->metal1.push_back(gnd);
}

Void autoLayout::nActiveSet(void)
{
	material nAct("nActive", NACTIVE, 0, 0, (float)(myComponent->width), (float)(28));
	myComponent->nActive.push_back(nAct);
}

Void autoLayout::paraSet(void)
{
	myComponent->expr = msclr::interop::marshal_as< std::string >(parent->textBoxPoly->Text);// get the polies
	for(unsigned int i=0; i<myComponent->expr.size(); i++)
	{
		if(myComponent->expr[i] == '%')
		{
			myComponent->gapNum++;
		}
		else
		{
			myComponent->polyNum++;
			
		}
	}
	myComponent->activeNum = myComponent->gapNum + 1;
	myComponent->width = (float)((8 * myComponent->polyNum + 7 * myComponent->gapNum + 4) * lambda + 12 * lambda);
	myComponent->ratio = (int)min(formW / myComponent->width, formH / myComponent->height);// amplify ratio
	offsetX = (formW - myComponent->ratio * myComponent->width) / 2;
	offsetY = (formH - myComponent->ratio * myComponent->height) / 2;
}

Void autoLayout::drawLayout(void)
{
	for(unsigned int i=0; i<myComponent->nActive.size(); i++)
	{
		FillMaterial(myComponent->nActive[i]);
	}
	for(unsigned int i=0; i<myComponent->activeP.size(); i++)
	{
		FillMaterial(myComponent->activeP[i]);
		FillMaterial(myComponent->activeN[i]);
	}
	for(unsigned int i=0; i<myComponent->pselect.size(); i++)
	{
		FillMaterial(myComponent->pselect[i]);
	}
	for(unsigned int i=0; i<myComponent->nselect.size(); i++)
	{
		FillMaterial(myComponent->nselect[i]);
	}
	for(unsigned int i=0; i<myComponent->metal1.size(); i++)
	{
		FillMaterial(myComponent->metal1[i]);
		DrawStringString(myComponent->metal1[i].name, myComponent->metal1[i].startX * myComponent->ratio, myComponent->metal1[i].startY * myComponent->ratio);
	}
	for(unsigned int i=0; i<myComponent->poly.size(); i++)
	{
		FillMaterial(myComponent->poly[i]);
		DrawStringString(myComponent->poly[i].name, myComponent->poly[i].startX * myComponent->ratio, myComponent->poly[i].startY * myComponent->ratio);
	}
	drawAllPath();
	for(unsigned int i=0; i<myComponent->via.size(); i++)
	{
		FillMaterial(myComponent->via[i]);
	}
	for(unsigned int i=0; i<myComponent->m2Select.size(); i++)
	{
		FillMaterial(myComponent->m2Select[i]);
	}
	for(unsigned int i=0; i<myComponent->m1Select.size(); i++)
	{
		FillMaterial(myComponent->m1Select[i]);
	}
	ShowNets();
}

/**********************   Draw the nets to the region   ***************************/
Void autoLayout::ShowNets(void)//System::Drawing::Rectangle^ rect)
{
	for(unsigned int i=0; i<myComponent->net.size(); i++)
	{
		for(unsigned int j=0; j<myComponent->net[i].size(); j++)
			DrawStringString(myComponent->net[i][j]->name, (myComponent->net[i][j]->startX - 2) * myComponent->ratio,
			(myComponent->net[i][j]->startY - 2) * myComponent->ratio);
	}
}

/**********************   Draw the path to the region   ***************************/
Void autoLayout::FillPath(path *temp)//System::Drawing::Rectangle^ rect)
{
	if(temp->M1orM2 == 2)// metal 2
	{
		material mtmp(temp->name, METAL2, (float)(temp->startX - wireWidth/2 * lambda), (float)(temp->startY - wireWidth/2 * lambda),
			(float)(temp->endX - temp->startX + wireWidth * lambda), (float)(wireWidth * lambda));
		FillMaterial(mtmp);
		//DrawStringString(mtmp.name, mtmp.startX * myComponent->ratio, mtmp.startY * myComponent->ratio);
	}
	else
	{
		material mtmp(temp->name, METAL1, (float)(temp->startX - wireWidth/2 * lambda), (float)(temp->startY - wireWidth/2 * lambda),
			(float)(wireWidth * lambda), (float)(temp->endY - temp->startY + wireWidth * lambda));
		FillMaterial(mtmp);
		//DrawStringString(mtmp.name, mtmp.startX * myComponent->ratio, mtmp.startY * myComponent->ratio);
	}
}

/**********************   Draw the module to the region   ***************************/
Void autoLayout::FillMaterial(material temp)//System::Drawing::Rectangle^ rect)
{
	material *m = new material;
	m = temp.amplify((float)myComponent->ratio);
	System::Drawing::Color c;// choose a color
	float gap;
	int dir = 0;
	switch (m->Color)
	{
	case IDLE:
		c = Color::Black;
		gap = 10;
		dir = 0;
		break;
	case POLY:
		c = Color::FromArgb(255, 255, 0, 0);
		gap = lineGap1;
		dir = 0;
		break;
	case METAL1:
		c = Color::Blue;
		gap = lineGap3;
		dir = 1;
		break;
	case METAL2:
		c = Color::DeepPink;
		gap = lineGap3;
		dir = 2;
		break;
	case M1ToM2:
		c = Color::DeepPink;
		dir = 0;
		break;
	case NACTIVE:
		c = Color::FromArgb(255, 0, 255, 0);
		gap = lineGap3;
		dir = 1;
		break;
	case ACTIVE:
		c = Color::FromArgb(255, 0, 255, 0);
		gap = lineGap3;
		dir = 2;
		break;
	case PSELECT:
		c = Color::FromArgb(255, 255, 165, 0);
		dir = -1;
		break;
	case NSELECT:
		c = Color::FromArgb(255, 0, 255, 0);
		dir = -1;
		break;
	case VIA:
		c = Color::Black;
		dir = 0;
		break;
	}
	Graphics^ formGraphics;
	formGraphics = pictureBox1->CreateGraphics();
	Pen^ pen = gcnew Pen(c);
	
	vector<float> startX;
	vector<float> startY;
	vector<float> endX;
	vector<float> endY;
	if(dir == 2)
	{
		float recordy = 0;
		for(int i = 0; float(i*gap) < m->H; i++)
		{
			startX.push_back(m->startX);
			startY.push_back(m->startY + i*gap);
		}
		//recordy = m.startY + m.H - startY[startY.size()-1];// record the remained length;
		float recordx = 0;
		for(int i = 0; float(i*gap) < m->W; i++)
		{
			startX.push_back(recordy + m->startX + i*gap);
			startY.push_back(m->startY + m->H);
			endX.push_back(m->startX + i*gap);
			endY.push_back(m->startY);
		}
		//recordx = m.startX + m.W - startX[startX.size()-1];// record the remained length;
		for(int i = 0; float(i*gap) < m->H; i++)
		{
			endX.push_back(m->startX + m->W);
			endY.push_back(recordx + m->startY + i*gap);
		}
		unsigned int sum = startX.size();
		for(unsigned int i=0; i<sum;i++)
			DrawLineLine(c, startX[i], startY[i], endX[i], endY[i]);
	}
	else if( dir == 1)
	{
		float recordx = 0;
		for(int i = 0; float(i*gap) < m->W; i++)
		{
			startX.push_back(m->startX + m->W - i*gap);
			startY.push_back(m->startY);
		}
		//recordx = startX[startX.size()-1] - m.startX;// record the remained length;
		float recordy = 0;
		for(int i = 0; float(i*gap) < m->H; i++)
		{
			startX.push_back(m->startX);
			startY.push_back(recordx + m->startY + i*gap);
			endX.push_back(m->startX + m->W);
			endY.push_back(m->startY + i*gap);
		}
		//recordy = m.startY + m.H - startY[startY.size()-1];
		for(int i = 0; float(i*gap) < m->W; i++)
		{
			endX.push_back(m->startX + m->W - recordy - i*gap);
			endY.push_back(m->startY + m->H);
		}
		unsigned int sum = startX.size();
		for(unsigned int i=0; i<sum;i++)
			DrawLineLine(c, startX[i], startY[i], endX[i], endY[i]);
	}
	else if(dir == 0)
	{
		FillRectangleRectangle(c, offsetX + m->startX, offsetY + m->startY , m->W , m->H);
	}
	formGraphics = pictureBox1->CreateGraphics();
	formGraphics->DrawRectangle(pen, offsetX + m->startX, offsetY + m->startY , m->W , m->H);
	delete pen;
	delete formGraphics;
}

/**********************   Draw the module to the region   ***************************/
Void autoLayout::FillRectangleRectangle(Color clr, float a, float b, float c, float d)//System::Drawing::Rectangle^ rect)
{
	SolidBrush^ Brush = gcnew SolidBrush(clr);;
	Graphics^ formGraphics;
	formGraphics = pictureBox1->CreateGraphics();
	formGraphics->FillRectangle(Brush, a, b, c, d);
	delete Brush;
	delete formGraphics;
}

/**********************   Draw the module to the region   ***************************/
Void autoLayout::DrawRectangleRectangle(float a, float b, float c, float d)//System::Drawing::Rectangle^ rect)
{
	Pen^ pen = gcnew Pen(Color::Blue);
	Graphics^ formGraphics;
	formGraphics = pictureBox1->CreateGraphics();
	formGraphics->DrawRectangle(pen, a, b, c, d);
	delete pen;
	delete formGraphics;
}

/**********************   Draw the line to the region   ***************************/
Void autoLayout::DrawLineLine(Color clr, float a, float b, float c, float d)
{
	System::Drawing::Pen^ pen =gcnew System::Drawing::Pen(clr);
	System::Drawing::Graphics^ formGraphics;
	formGraphics = pictureBox1->CreateGraphics();
	formGraphics->DrawLine(pen, offsetX + a, offsetY + b, offsetX + c, offsetY + d);
	delete pen;
	delete formGraphics;
}

/**********************   Draw String to the region   ***************************/
Void autoLayout::DrawStringString(string s, float a, float b)
{
	String ^tmp = gcnew String(s.c_str());
	System::Drawing::Font ^drawFont = gcnew System::Drawing::Font("Arial", 12);
	SolidBrush ^Brush = gcnew SolidBrush( Color::White);
	PointF pt = PointF(offsetX + a, offsetY + b);
	System::Drawing::Graphics^ formGraphics;
	formGraphics = pictureBox1->CreateGraphics();
	formGraphics->DrawString(tmp, drawFont, Brush, pt);
	delete Brush;
	delete formGraphics;
}
