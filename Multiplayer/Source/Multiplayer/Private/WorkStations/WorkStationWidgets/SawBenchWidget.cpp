// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkStations/WorkStationWidgets/SawBenchWidget.h"

void USawBenchWidget::UpdateLogCount(int32 LogCount)
{
	if (GEngine) {

		//GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Green, FString::Printf(TEXT("Update Log Count: %d"), LogCount));

		// If There Are Logs
		if (LogCount != -1) {

			// Set Log Count Text To The Number Of Logs
			LogCountText->SetText(FText::FromString(FString::FromInt(LogCount + 1)));

			LogImage->SetBrushFromTexture(LogTexture);
		}
		else {

			// Set Log Count Text To Empty String So It Doesn't Show Up
			LogCountText->SetText(FText::FromString(""));

			LogImage->SetBrushFromTexture(NoLogTexture);
		}
	}
}

void USawBenchWidget::UpdateProgressBar(float Progress)
{
	SawProgressBar->SetPercent(Progress);
}

void USawBenchWidget::UpdateBucketUI(bool bHasBucket)
{
	// Is There A Bucketing Waiting To Be Picked Up
	if (bHasBucket) {

		// Yes - Show Bucket Image
		BucketImage->SetBrushFromTexture(BowlTexture);
	}
	else {

		// No - Show Emptry Bucket Image
		BucketImage->SetBrushFromTexture(NoBowlTexture);
	}
}
