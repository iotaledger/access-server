/*
 *  This file is part of the Frost distribution
 *  (https://github.com/xainag/frost)
 *
 *  Copyright (c) 2019 XAIN AG.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

package com.de.xain.emdac.delegation;

import android.databinding.DataBindingUtil;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;

import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.ViewMultipleRuleBinding;
import com.de.xain.emdac.databinding.ViewRuleLocationBinding;
import com.de.xain.emdac.databinding.ViewRuleSpeedLimitBinding;
import com.de.xain.emdac.databinding.ViewRuleTimeBinding;
import com.de.xain.emdac.utils.Constants;
import com.de.xain.emdac.utils.ui.UiUtils;

import java.text.SimpleDateFormat;
import java.util.List;
import java.util.Locale;

public class RulesAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

    private static final int ADD_RULE_VIEW_TYPE = 0;
    private static final int RULE_LOCATION_VIEW_TYPE = 1;
    private static final int RULE_TIME_VIEW_TYPE = 2;
    private static final int RULE_MULTIPLE_VIEW_TYPE = 3;
    private static final int RULE_SPEED_LIMIT_VIEW_TYPE = 4;

    private final List<Rule> mRuleList;
    private final OnItemClickListener mListener;

    public RulesAdapter(List<Rule> ruleList, OnItemClickListener listener) {
        mRuleList = ruleList;
        mListener = listener;
    }

    @Override
    public int getItemViewType(int position) {
        if (position == mRuleList.size()) { // not decreased by 1 because number of items is mRuleList.size() - 1 + 1
            return ADD_RULE_VIEW_TYPE;
        }

        Rule rule = mRuleList.get(position);
        if (rule instanceof LocationRule) {
            return RULE_LOCATION_VIEW_TYPE;
        } else if (rule instanceof TimeRule) {
            return RULE_TIME_VIEW_TYPE;
        } else if (rule instanceof MultipleRule) {
            return RULE_MULTIPLE_VIEW_TYPE;
        } else if (rule instanceof SpeedLimitRule) {
            return RULE_SPEED_LIMIT_VIEW_TYPE;
        }

        return ADD_RULE_VIEW_TYPE;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int viewType) {
        switch (viewType) {

            case RULE_LOCATION_VIEW_TYPE:
                return new LocationRuleViewHolder(
                        DataBindingUtil.inflate(
                                LayoutInflater.from(viewGroup.getContext()),
                                R.layout.view_rule_location,
                                viewGroup,
                                false));
            case RULE_TIME_VIEW_TYPE:
                return new TimeRuleViewHolder(
                        DataBindingUtil.inflate(
                                LayoutInflater.from(viewGroup.getContext()),
                                R.layout.view_rule_time,
                                viewGroup,
                                false));
            case RULE_SPEED_LIMIT_VIEW_TYPE:
                return new SpeedLimitRuleViewHolder(
                        DataBindingUtil.inflate(
                                LayoutInflater.from(viewGroup.getContext()),
                                R.layout.view_rule_speed_limit,
                                viewGroup,
                                false));
            case RULE_MULTIPLE_VIEW_TYPE:
                return new MultipleRuleViewHolder(
                        DataBindingUtil.inflate(
                                LayoutInflater.from(viewGroup.getContext()),
                                R.layout.view_multiple_rule,
                                viewGroup,
                                false));
            case ADD_RULE_VIEW_TYPE:
            default:
                return new AddRuleViewHolder(
                        LayoutInflater.from(viewGroup.getContext()).inflate(
                                R.layout.add_rule_list_item,
                                viewGroup,
                                false));
        }
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder viewHolder, int position) {
        Rule rule;
        switch (getItemViewType(position)) {
            case RULE_LOCATION_VIEW_TYPE:
                rule = mRuleList.get(position);
                if (rule instanceof LocationRule)
                    ((LocationRuleViewHolder) viewHolder).bind((LocationRule) rule, mListener);
                break;
            case RULE_TIME_VIEW_TYPE:
                rule = mRuleList.get(position);
                if (rule instanceof TimeRule)
                    ((TimeRuleViewHolder) viewHolder).bind((TimeRule) rule, mListener);
                break;
            case RULE_MULTIPLE_VIEW_TYPE:
                rule = mRuleList.get(position);
                if (rule instanceof MultipleRule)
                    ((MultipleRuleViewHolder) viewHolder).bind(rule, mListener);
                break;
            case RULE_SPEED_LIMIT_VIEW_TYPE:
                rule = mRuleList.get(position);
                if (rule instanceof SpeedLimitRule)
                    ((SpeedLimitRuleViewHolder) viewHolder).bind(((SpeedLimitRule) rule), mListener);
                break;
            case ADD_RULE_VIEW_TYPE:
                ((AddRuleViewHolder) viewHolder).mButton.setOnClickListener(__ -> mListener.onAddRuleItemClick());
                break;
        }
    }

    @Override
    public int getItemCount() {
        return mRuleList.size() + 1;
    }

    public interface OnItemClickListener {
        void onAddRuleItemClick();

        void onRuleSelect(Rule rule, RuleType type);
    }

    static private abstract class CustomViewHolder extends RecyclerView.ViewHolder {
        protected ImageButton mImageButton;

        public CustomViewHolder(@NonNull View itemView) {
            super(itemView);
        }

        protected void setImage(@DrawableRes int resId) {
            if (mImageButton == null) return;
            Drawable mapImage = UiUtils.getCompatDrawable(mImageButton.getContext(), resId);
            if (mapImage != null) {
                mapImage = mapImage.mutate();
                mapImage.setTint(UiUtils.getColorFromAttr(mImageButton.getContext(), R.attr.button_1_image_color, Color.GRAY));
                mImageButton.setBackground(mapImage);
            }
        }
    }

    private static class TimeRuleViewHolder extends CustomViewHolder {
        final TextView mFromTextView;
        final TextView mUntilTextView;

        private TimeRuleViewHolder(ViewRuleTimeBinding binding) {
            super(binding.getRoot());
            mImageButton = binding.imageArrow;
            mFromTextView = binding.labelFrom;
            mUntilTextView = binding.labelUntil;
            setImage(R.drawable.ic_clock);
        }

        private void bind(TimeRule rule, OnItemClickListener listener) {
            SimpleDateFormat format = new SimpleDateFormat(Constants.DATE_AND_TIME_FORMAT, Locale.getDefault());
            mFromTextView.setText(format.format(rule.getFromDate()));
            mUntilTextView.setText(format.format(rule.getUntilDate()));
            itemView.setOnClickListener(__ -> listener.onRuleSelect(rule, RuleType.SINGLE));
        }
    }

    private static class SpeedLimitRuleViewHolder extends CustomViewHolder {
        final TextView mSpeedLimitTextView;

        private SpeedLimitRuleViewHolder(ViewRuleSpeedLimitBinding binding) {
            super(binding.getRoot());
            mImageButton = binding.imageArrow;
            mSpeedLimitTextView = binding.labelSpeedLimit;
            setImage(R.drawable.ic_speedometer);
        }

        private void bind(SpeedLimitRule rule, OnItemClickListener listener) {
            StringBuilder builder = new StringBuilder();
            builder.append(rule.getSpeedLimit());
            builder.append(" ");
            builder.append(rule.getUnit());
            mSpeedLimitTextView.setText(builder.toString());
            itemView.setOnClickListener(__ -> listener.onRuleSelect(rule, RuleType.SINGLE));
        }
    }

    private static class LocationRuleViewHolder extends CustomViewHolder {
        final TextView mLatiudeTextView;
        final TextView mLongitudeTextView;
        final TextView mRadiusTextView;
        final TextView mUnitTextView;

        private LocationRuleViewHolder(ViewRuleLocationBinding binding) {
            super(binding.getRoot());
            mImageButton = binding.imageArrow;
            mLatiudeTextView = binding.labelLatitude;
            mLongitudeTextView = binding.labelLongitude;
            mRadiusTextView = binding.labelRadius;
            mUnitTextView = binding.labelUnits;
            setImage(R.drawable.ic_map);
        }

        private void bind(LocationRule rule, OnItemClickListener listener) {
            mLatiudeTextView.setText(String.valueOf(rule.getLatitude()));
            mLongitudeTextView.setText(String.valueOf(rule.getLongitude()));
            mRadiusTextView.setText(String.valueOf(rule.getRadius()));
            mUnitTextView.setText(rule.getUnit().getShortStringValue());
            itemView.setOnClickListener(__ -> listener.onRuleSelect(rule, RuleType.SINGLE));
        }
    }

    private static class AddRuleViewHolder extends RecyclerView.ViewHolder {
        final Button mButton;

        private AddRuleViewHolder(View view) {
            super(view);
            mButton = view.findViewById(R.id.button);
            mButton.setText(view.getContext().getString(R.string.button_add_rule));
            Drawable buttonImage = UiUtils.getCompatDrawable(view.getContext(), R.drawable.ic_add_circle);
            if (buttonImage != null) {
                buttonImage = buttonImage.mutate();
                buttonImage.setTint(UiUtils.getColorFromAttr(view.getContext(), R.attr.button_1_image_color, Color.GRAY));
                mButton.setCompoundDrawablesWithIntrinsicBounds(buttonImage, null, null, null);
            }
        }
    }

    private static class MultipleRuleViewHolder extends RecyclerView.ViewHolder {

        private MultipleRuleViewHolder(ViewMultipleRuleBinding binding) {
            super(binding.getRoot());
        }

        private void bind(Rule rule, OnItemClickListener listener) {
            itemView.setOnClickListener(__ -> listener.onRuleSelect(rule, RuleType.MULTIPLE));
        }
    }
}
