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

package com.de.xain.emdac.api.model;


import android.support.annotation.Nullable;

import com.de.xain.emdac.R;
import com.de.xain.emdac.utils.ResourceProvider;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Command implements CommandListItem {

    /**
     * Builder class responsible for creating instance of Command.
     */
    public static class Builder {
        private CommandAction[] mActionList;
        private CommandAction mActiveAction;
        private String mState;
        private boolean mDeletable;

        public Builder setDeletable(boolean deletable) {
            this.mDeletable = deletable;
            return this;
        }

        public Builder setActionList(CommandAction[] actionList) {
            this.mActionList = actionList;
            return this;
        }

        public Builder setActiveAction(CommandAction activeAction) {
            this.mActiveAction = activeAction;
            return this;
        }

        public Builder setState(String state) {
            this.mState = state;
            return this;
        }

        public Command create() throws CommandException {
            return new Command(this);
        }
    }

    public class CommandException extends Exception {
        CommandException(String message) {
            super(message);
        }
    }

    public class KnownState {
        public static final String COMMAND_STATE_UNLOCKED = "unlock";
        public static final String COMMAND_STATE_LOCKED = "lock";
        public static final String ALARM_STATE_ON = "on";
        public static final String ALARM_STATE_OFF = "off";
    }

    private final CommandAction[] mActionList;
    private CommandAction mActiveAction;
    private String mState;

    /**
     * Field to determine if command can be deleted by user.
     */
    private boolean mDeletable = false;


    public void setState(String state) {
        mState = state;
        setActiveAction(getCommandActionForState(state));
    }

    public Command(CommandAction[] actionList, String state) throws CommandException {
        if (actionList.length < 1) {
            throw new CommandException("Action list can't be empty");
        }
        this.mActionList = actionList;
        this.mState = state;

        this.mActiveAction = getCommandActionForState(state);
    }

    private Command(Command.Builder builder) throws CommandException {
        if (builder.mActionList.length < 1) {
            throw new CommandException("Action list can't be empty");
        }
        this.mDeletable = builder.mDeletable;
        this.mActionList = builder.mActionList;
        this.mState = builder.mState;
        this.mActiveAction = getCommandActionForState(builder.mState);
    }

    public Command(Command command) {
        this.mActionList = Arrays.copyOf(command.mActionList, command.mActionList.length);
        this.mActiveAction = command.mActiveAction;
        this.mState = command.mState;
    }

    public boolean isAllPaid() {
        for (CommandAction action : mActionList)
            if (!action.isPaid()) return false;
        return true;
    }

    public boolean isDeletable() {
        return mDeletable;
    }

    public void setDeletable(boolean deletable) {
        mDeletable = deletable;
    }

    public CommandAction[] getActionList() {
        return mActionList;
    }

    public CommandAction getActiveAction() {
        return mActiveAction;
    }

    public void setActiveAction(CommandAction activeAction) {
        this.mActiveAction = activeAction;
    }

    public String getState() {
        return mState;
    }

    @Override
    public String getHeaderName() {
        return mActiveAction.getHeaderName();
    }

    @Override
    public String getActionName() {
        return mActiveAction.getActionName();
    }

    @Override
    public int getHeaderImageResId() {
        switch (mActiveAction.getAction().toLowerCase()) {
            case CommandAction.KnownCommandAction.ALARM_ON:
            case CommandAction.KnownCommandAction.ALARM_OFF:
                return R.drawable.ic_alarm_on;
            case CommandAction.KnownCommandAction.HONK_ACTION:
                return R.drawable.ic_car_horn;
            case CommandAction.KnownCommandAction.OPEN_TRUNK_ACTION:
                return R.drawable.ic_car_trunk_open;
            case CommandAction.KnownCommandAction.OPEN_DOOR_ACTION:
            case CommandAction.KnownCommandAction.CLOSE_DOOR_ACTION:
                return R.drawable.ic_locked;
            case CommandAction.KnownCommandAction.START_ENGINE_ACTION:
                return R.drawable.ic_start_engine;
            default:
                return R.drawable.ic_unknown;
        }
    }

    @Override
    public int getStatusImageResId() {
        switch (mActiveAction.getAction().toLowerCase()) {
            case CommandAction.KnownCommandAction.HONK_ACTION:
                return R.drawable.ic_car_horn;
            case CommandAction.KnownCommandAction.OPEN_TRUNK_ACTION:
                if (mState.equalsIgnoreCase(KnownState.COMMAND_STATE_UNLOCKED)) {
                    return R.drawable.ic_car_trunk_open;
                } else {
                    return R.drawable.ic_car_trunk_closed;
                }
            case CommandAction.KnownCommandAction.OPEN_DOOR_ACTION:
            case CommandAction.KnownCommandAction.CLOSE_DOOR_ACTION:
                if (mState.equalsIgnoreCase(KnownState.COMMAND_STATE_UNLOCKED)) {
                    return R.drawable.ic_unlocked;
                } else {
                    return R.drawable.ic_locked;
                }
            case CommandAction.KnownCommandAction.ALARM_ON:
            case CommandAction.KnownCommandAction.ALARM_OFF:
                if (mState.equalsIgnoreCase(KnownState.ALARM_STATE_ON)) {
                    return R.drawable.ic_alarm_on;
                } else {
                    return R.drawable.ic_alarm_off;
                }
            case CommandAction.KnownCommandAction.START_ENGINE_ACTION:
                return R.drawable.ic_start_engine;
            default:
                return R.drawable.ic_unknown;
        }
    }

    private CommandAction getCommandActionForState(String state) {

        if (state.equalsIgnoreCase(KnownState.ALARM_STATE_ON)) {
            for (CommandAction action : getActionList()) {
                if (action.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.ALARM_OFF)) {
                    return action;
                }
            }
        }

        if (state.equalsIgnoreCase(KnownState.ALARM_STATE_OFF)) {
            for (CommandAction action : getActionList()) {
                if (action.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.ALARM_ON)) {
                    return action;
                }
            }
        }

        // if state is locked, try to find any action that opens (open_door, open_trunk)
        if (state.equalsIgnoreCase(KnownState.COMMAND_STATE_LOCKED)) {
            for (CommandAction action : getActionList()) {
                if (action.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.OPEN_DOOR_ACTION) || action.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.OPEN_TRUNK_ACTION)) {
                    return action;
                }
            }
        } else {
            // state is different from locked, so try to find any action that closes (close_door)
            for (CommandAction action : getActionList()) {
                if (action.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.CLOSE_DOOR_ACTION)) {
                    return action;
                }
            }
        }
        // if searching for appropriate action failed, return first action
        return getActionList()[0];
    }

    public void didExecute(CommandAction commandAction) {
        if (commandAction.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.OPEN_DOOR_ACTION) || commandAction.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.OPEN_TRUNK_ACTION)) {
            setState(KnownState.COMMAND_STATE_UNLOCKED);
        } else if (commandAction.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.ALARM_ON)) {
            setState(KnownState.ALARM_STATE_ON);
        } else if (commandAction.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.ALARM_OFF)) {
            setState(KnownState.ALARM_STATE_OFF);
        } else {
            setState(KnownState.COMMAND_STATE_LOCKED);
        }
    }

    @Override
    public boolean equals(@Nullable Object obj) {
        if (obj instanceof Command) {
            Command command = (Command) obj;
            //noinspection ConstantConditions
            if (command == null) return false;

            List<CommandAction> a = Arrays.asList(this.mActionList);
            List<CommandAction> b = Arrays.asList(command.mActionList);
            return a.containsAll(b) && b.containsAll(a);
        } else {
            return false;
        }
    }

    /**
     * Make list of command by from passed list of actions.
     *
     * @param actions List of actions which can correlate to single command.
     * @return List of commands.
     */
    public static List<Command> makeListOfCommands(List<CommandAction> actions, ResourceProvider resourceProvider) {
        List<Command> commands = new ArrayList<>();
        if (actions.isEmpty()) return commands;

        List<CommandAction> lockUnlockActions = new ArrayList<>();
        List<CommandAction> alarmActions = new ArrayList<>();
        for (CommandAction action : actions) {
            if ((action.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.OPEN_DOOR_ACTION) || action.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.CLOSE_DOOR_ACTION))) {
                if (action.getActionNameResId() == 0) {
                    action.setActionName(resourceProvider.getString(R.string.unknown_action));
                } else {
                    action.setActionName(resourceProvider.getString(action.getActionNameResId()));
                }
                if (action.getHeaderNameResId() == 0) {
                    action.setHeaderName(resourceProvider.getString(R.string.unknown_command));
                } else {
                    action.setHeaderName(resourceProvider.getString(action.getHeaderNameResId()));
                }
                lockUnlockActions.add(action);
            } else if ((action.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.ALARM_ON) || action.getAction().equalsIgnoreCase(CommandAction.KnownCommandAction.ALARM_OFF))) {
                if (action.getActionNameResId() == 0) {
                    action.setActionName(resourceProvider.getString(R.string.unknown_action));
                } else {
                    action.setActionName(resourceProvider.getString(action.getActionNameResId()));
                }
                if (action.getHeaderNameResId() == 0) {
                    action.setHeaderName(resourceProvider.getString(R.string.unknown_command));
                } else {
                    action.setHeaderName(resourceProvider.getString(action.getHeaderNameResId()));
                }
                alarmActions.add(action);
            } else {
                try {
                    commands.add(new Command(new CommandAction[]{action}, Command.KnownState.COMMAND_STATE_LOCKED));
                } catch (Exception ignored) {
                }
            }
        }
        try {
            commands.add(0, new Command(lockUnlockActions.toArray(new CommandAction[0]), KnownState.COMMAND_STATE_LOCKED));
            commands.add(new Command(alarmActions.toArray(new CommandAction[0]), KnownState.ALARM_STATE_OFF));
        } catch (Command.CommandException ignored) {
        }

        return commands;
    }


}