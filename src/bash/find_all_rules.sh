# Taken from /lib/udev/write_net_rules

RULES_FILE='/etc/udev/rules.d/70-persistent-net.rules'

. /lib/udev/rule_generator.functions

# Find all rules matching a key (with action) and a pattern.
find_all_rules() {
        local key="$1"
        local linkre="$2"
        local match="$3"

        local search='.*[[:space:],]'"$key"'"('"$linkre"')".*'
        echo $(sed -n -r -e 's/^#.*//' -e "${match}s/${search}/\1/p" \
                $RO_RULES_FILE \
                $([ -e $RULES_FILE ] && echo $RULES_FILE) \
                2>/dev/null)
}

interface_name_taken() {
        local value="$(find_all_rules 'NAME=' $INTERFACE)"
        if [ "$value" ]; then
                return 0
        else
                return 1
        fi
}
